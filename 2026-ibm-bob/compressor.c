/*
 * ===========================================================================
 *  compressor.c — LZ77 + Huffman file compressor
 *
 *  This is an open source code developed to test IBM Bob functionalities.
 *
 *  License : MIT
 *  Authors : contributors
 * ===========================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define WINDOW_SIZE 512
#define LOOK_AHEAD  256
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define TRUE 1
#define FALSE 0

typedef uint16_t LZ77Token;

struct LZ77Frequence {
    LZ77Token token;
    int       count;
};

typedef struct LZ77Frequence LZ77Frequence;

struct HuffEntry {
    uint8_t  symbol;   // the byte value
    uint32_t freq;     // frequency (LE)
};

typedef struct HuffEntry HuffEntry;

/*
 * ===========================================================================
 *  FILE FORMAT  (.zipc)
 * ===========================================================================
 * *  Offset  Size  Field
 *  ------    ----  -------------------------------------------------
 *   0        4      MAGIC          — 0x5A 0x49 0x50 0x43  ("ZIPC")
 *   4        1      VERSION        — 0x01
 *   5        8      ORIG_SIZE      — original file size in bytes (uint64_t LE)
 *  13        N+1    ORIG_NAME      — original filename (including null terminator)
 *  14+N      2      HUFF_ENTRIES   — number of entries in the Huffman table (uint16_t LE)
 *  16+N      5*E    HUFF_TABLE     — E entries of { symbol(uint8), freq(uint32 LE) }
 *  16+N+5E   2      DATA_BYTES     — number of bytes in the compressed data block (LE)
 *  18+N+5E   1      PADDING_BITS   — how many bits in the last byte are padding (0..7)
 *  19+N+5E   *      COMPRESSED_DATA — Huffman-encoded LZ77 token stream (packed bits)
 */
struct ZipcHeader {
    uint32_t   magic;            // 0x5A495043 ("ZIPC")
    uint8_t    version;          // 0x01
    uint64_t   orig_size;        // original file size in bytes (LE)
    char*      orig_name;        // original file name (null terminator)
    uint16_t   huff_entries;     // number of entries in the Huffman table (LE)
    HuffEntry* huff_table;       // pointer to the Huffman table entries (array of HuffEntry)
    /* FIXME: data_bytes is uint16_t which limits compressed output to 65535 bytes.
     *        Change to uint64_t to support files of arbitrary size.
     *        Also update the file-format table above accordingly (field size 2 → 8). */
    uint16_t   data_bytes;       // number of bytes in the compressed data block (LE)
    uint8_t    padding_bits;     // how many bits in the last byte are padding (0..7)
    uint8_t*   compressed_data;  // pointer to the compressed data block (array of bytes)
};

typedef struct ZipcHeader ZipcHeader;

/*
 * ===========================================================================
 *  TWO-STAGE PIPELINE
 * ===========================================================================
 *
 *  COMPRESS:   raw bytes  →  [LZ77]  →  token stream  →  [Huffman]  →  bit stream
 *  DECOMPRESS: bit stream →  [Huffman decode]  →  token stream  →  [LZ77 replay]  →  raw bytes
 *
 * ===========================================================================
 */

// Huffman-related data structures and helper functions

// HuffNode - one node in the Huffman binary tree

/* REVIEW: HuffNode uses 'left' and 'right' for two different purposes that
 *         conflict with each other:
 *
 *   — In build_huffman_tree() they are used as linked-list prev/next pointers
 *     to chain all nodes together.
 *   — In the final Huffman tree they must be the 0-branch (left) and
 *     1-branch (right) children used during encoding and decoding.
 *
 *   These two roles cannot share the same two pointers.
 *   Solution: add separate 'prev' and 'next' pointers for the list phase,
 *   OR build the list externally (e.g. a pointer array) and keep left/right
 *   exclusively for the tree structure. */
struct HuffNode {
    LZ77Token *symbols;     // the byte value (meaningful only in leaf nodes)
    uint32_t   num_symbols;    // number of symbols in the leaf node
    unsigned int  freq;     // frequency / combined weight
    struct HuffNode *left;  // left  child (0-branch), NULL in a leaf
    struct HuffNode *right; // right child (1-branch), NULL in a leaf
};

typedef struct HuffNode HuffNode;

// HuffCode - the variable-length bit-code assigned to one symbol
struct HuffCode {
    unsigned char symbol;
    unsigned char bits[32]; // the actual bits, one per array cell (0 or 1)
    int           length;   // how many bits are valid
};

typedef struct HuffCode HuffCode;

// BitWriter / BitReader — helpers to write/read individual bits into/from a
// byte buffer, packing 8 bits per byte.
struct BitWriter {
    unsigned char *buf;     // output byte buffer
    int            byte_pos;
    int            bit_pos; // current bit position within the current byte (0..7)
};

typedef struct BitWriter BitWriter;

struct BitReader {
    const unsigned char *buf; // input byte buffer
    int            byte_pos;
    int            bit_pos;   // current bit position within the current byte (0..7)
};

typedef struct BitReader BitReader;

/*
 * ===========================================================================
 *  QUICK SORT FOR LZ77 FREQUENCY TABLE
 * ===========================================================================
 *
 *  The partition() and quicksort() functions are used to sort an array of
 *  LZ77Frequence structures in ascending order based on the count field.
 *
 * ===========================================================================
 */

// Partition function for quicksort (used in Huffman tree construction)
int partition(LZ77Frequence *arr, int low, int high) {
    LZ77Frequence pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j].count < pivot.count) {
            i++;
            LZ77Frequence temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    LZ77Frequence temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

// Quicksort partition function for LZ77Frequence array (ascending order by count)
void quicksort(LZ77Frequence *arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

/* ---------------------------------------------------------------------------
 * HUFFMAN HELPER FUNCTIONS  (implement these first, then wire into compress/decompress)
 * --------------------------------------------------------------------------- */

LZ77Frequence* build_freq_table(LZ77Token *tokens, int *count) {
    int entries_count = 0;
    char found = FALSE;
    int total = *count;
    LZ77Token entry;

    LZ77Frequence *all_tokens = (LZ77Frequence*)malloc(total * sizeof(LZ77Frequence));
    if (!all_tokens) return NULL;
    
    for (int idx = 0; idx < total; idx++) {
        entry = tokens[idx];

        /* REVIEW: 'found' must be reset to FALSE here, at the start of each
         *         outer iteration, before the inner search loop runs.
         *         Currently it is only reset inside the else-branch, meaning
         *         that if the previous token matched, 'found' stays TRUE and
         *         the very next token will never enter the !found branch even
         *         if it is a brand-new symbol. Add:   found = FALSE;   here. */
        int pos = 0;
        for (pos = 0; pos < entries_count; pos++) {
            if (all_tokens[pos].token == entry) {
                found = TRUE;
                break;
            }
        }

        if (!found) {
            all_tokens[entries_count].token = entry;
            all_tokens[entries_count].count = 1;
            entries_count++;
        }
        else {
            all_tokens[pos].count++;
            found = FALSE;
        }
    }

    quicksort(all_tokens, 0, entries_count - 1);

    *count = entries_count;
    return all_tokens;
}

HuffNode* build_huffman_tree(LZ77Frequence *freq_table, int count) {
    // Implementation goes here
    HuffNode *root = NULL, *leaf = NULL, *last = NULL;

    /* REVIEW: build_huffman_tree() is incomplete.
     *
     *   Step A — Build initial leaf nodes.
     *            Create one HuffNode per entry in freq_table. Store them in a
     *            separate pointer array (NOT using left/right, to avoid the
     *            conflict described on HuffNode above).
     *            Each leaf: symbols = &freq_table[pos].token, freq = freq_table[pos].count,
     *            left = NULL, right = NULL.
     *
     *   Step B — Merge loop (the actual Huffman algorithm).
     *            While more than one node remains in the array:
     *              1. The array is already sorted by freq (freq_table came from quicksort).
     *                 Take the first two entries (lowest freq) as left and right children.
     *              2. Allocate a new internal HuffNode:
     *                   internal->freq  = left->freq + right->freq
     *                   internal->left  = left
     *                   internal->right = right
     *                   internal->symbols = NULL  (not a leaf)
     *              3. Remove the two consumed nodes from the array.
     *              4. Insert the new internal node back into the array in sorted
     *                 position (insert so that freq order is maintained).
     *            The single node left in the array is the root.
     *
     *   Step C — Return root.
     *
     *   NOTE: The current linked-list construction below must be replaced entirely
     *         with the pointer array approach described above. */

    // Make a linked list of HuffNodes from the frequency table
    for (int pos = 0; pos < count; pos++) {
        leaf = (HuffNode*)malloc(sizeof(HuffNode));
        if (!leaf) return NULL;

        leaf->symbols = (LZ77Token*)malloc(sizeof(LZ77Token));
        if (!leaf->symbols) return NULL;

        *(leaf->symbols) = freq_table[pos].token;
        leaf->num_symbols = 1;
        leaf->freq = freq_table[pos].count;
        leaf->left = NULL;
        leaf->right = NULL;

        if (root == NULL) {
            root = leaf;
            last = leaf;
        }
        else {
            last->right = leaf;  /* REVIEW: 'right' used as linked-list next — conflicts
                                  *         with the tree's 1-branch child. See note above. */
            leaf->left = last;   /* REVIEW: 'left' used as linked-list prev — conflicts
                                  *         with the tree's 0-branch child. See note above. */
            last = leaf;
        }
    }

    return root;
}


 /*
 *
 *  build_huffman_tree(freq[256])  → HuffNode* (root)
 *    — Create one leaf HuffNode for every symbol whose freq > 0.
 *    — Use a min-heap (priority queue) ordered by freq.
 *    — Repeat until only one node remains in the heap:
 *        1. Pop the two nodes with the lowest freq (left, right).
 *        2. Create a new internal node whose freq = left->freq + right->freq.
 *        3. Set left/right children and push the new node back into the heap.
 *    — Return the last remaining node as the root.
 *
 *  generate_codes(root, HuffCode codes[256])
 *    — Traverse the tree recursively.
 *    — At each left branch append a 0 to the current code; at each right branch append a 1.
 *    — When a leaf is reached, store the accumulated code in codes[leaf->symbol].
 *
 *  free_huffman_tree(root)
 *    — Post-order recursive free() of every HuffNode.
 *
 *  write_bit(BitWriter*, bit)
 *    — Store one bit into the current byte of the buffer.
 *    — Advance bit_pos; when it reaches 8, advance byte_pos and reset bit_pos to 0.
 *
 *  read_bit(BitReader*)  → 0 or 1
 *    — Read one bit from the current byte.
 *    — Advance bit_pos; when it reaches 8, advance byte_pos and reset bit_pos to 0.
 * --------------------------------------------------------------------------- */


/* Forward declarations */
void compress(const char *input_path, const char *output_path);
void decompress(const char *input_path, const char *output_path);

/* -----------------------------------------------------------------------
 * usage
 *   Prints how to invoke the program and exits.
 * ----------------------------------------------------------------------- */
void usage(const char *program_name)
{
    printf("Usage:\n");
    printf("  %s -c <input_file> <output_file>   Compress a file\n", program_name);
    printf("  %s -d <input_file> <output_file>   Decompress a file\n", program_name);
}

/* -----------------------------------------------------------------------
 * compress
 *   TWO-STAGE COMPRESSION: LZ77 sliding window  →  Huffman encoding.
 *
 *   WINDOW LAYOUT (512 bytes total):
 *
 *     [ 0 ... 255 | 256 ... 511 ]
 *       ^BUFFER^    ^LOOK-AHEAD^
 *
 *   Two pointers both start at position 256:
 *     - 'base'  : the start of the look-ahead (left edge, stays at 256)
 *     - 'ahead' : moves right as bytes are read from the file
 *
 * ----------------------------------------------------------------------- */
void compress(const char *input_path, const char *output_path)
{
    /* ---- STAGE 1 : LZ77  ------------------------------------------------ */

    // Declaration of the 512-byte sliding window array.
    unsigned char window[512];

    // Two pointers (base and ahead), both starting at position 256.
    unsigned int base = 256, ahead = 256;

    // Open input_path for reading in binary mode ("rb").
    // Check that the file opened successfully.
    // Read the original file size (fseek to end, ftell, rewind).
    // Extract the base filename from input_path for the header.
    
    FILE *input_file = fopen(input_path, "rb");
    if (!input_file) {
        fprintf(stderr, "Failed to open input file: %s\n", input_path);
        exit(EXIT_FAILURE);
    }

    fseek(input_file, 0, SEEK_END);
    long original_file_size = ftell(input_file);
    rewind(input_file);

    // Declaring a dynamic byte array (e.g. malloc'd unsigned char*)
    // to accumulate the raw LZ77 token bytes before Huffman encodes them.
    // Also declaring a counter for how many bytes are stored.
    
    LZ77Token *token_array = (LZ77Token*)malloc(sizeof(LZ77Token) * original_file_size);
    size_t token_count = 0;
     if (!token_array) {
        fprintf(stderr, "Failed to allocate memory for token array.\n");
        exit(EXIT_FAILURE);
    }

    // INITIAL FILL of the look-ahead half.
    // Read bytes from the input file one by one into
    // window[256], window[257], ... advancing 'ahead'
    // for each byte read, until the look-ahead is full (ahead == 512)
    // or the file ends.

    size_t bytes_read = fread(window + LOOK_AHEAD, 1, WINDOW_SIZE - ahead, input_file);
    ahead += bytes_read;

    // MAIN LZ77 LOOP.
    // Continue while there is at least one byte in the look-ahead
    // (i.e. ahead > LOOK_AHEAD, meaning ahead > 256).
    while (ahead > LOOK_AHEAD) {
        unsigned char distance = 0, value = 0;
        unsigned char count = 0;
        char match_found = FALSE;

        // Read the first byte of the look-ahead: window[LOOK_AHEAD].
        // Search for it in the buffer, scanning from position 255
        // down to position 0 (or the earliest valid buffer byte).
        distance = 0;
        value = window[LOOK_AHEAD];
        for (int pos = LOOK_AHEAD - 1; pos >= base; pos--) {
            if (window[pos] == value) {
                match_found = TRUE;

                /* REVIEW: 'count' must be reset to 0 here, before the inner
                 *         length-counting loop below. Without this reset, the
                 *         length from the previous candidate position accumulates
                 *         into the current one, producing inflated lengths. */

                // Count the length of the match starting at this position.
                for (int idx = 0;
                    idx < LOOK_AHEAD - pos && window[pos + idx] == window[LOOK_AHEAD + idx]; idx++) {
                    count++;
                }

                /* REVIEW: the best-match comparison   count > value   is wrong.
                 *         At this point 'value' has already been overwritten with
                 *         the length of the previous best match (see the assignment
                 *         value = count  a few lines down).  Use a separate variable,
                 *         e.g. 'best_len', to track the longest match found so far,
                 *         and compare:   count > best_len   instead. */
                if (distance == 0 || count > value) {
                    distance = LOOK_AHEAD - pos;
                    value = count;
                }
            }
        }

        // NO MATCH FOUND:
        //     Append two bytes to the token array: { 0x00, window[base] }
        //     The first byte (0) signals "no match / literal".

        // MATCH FOUND:
        //     Extend the match: compare window[base+1], window[base+2], ...
        //     against the bytes that follow the found position in the buffer,
        //     as long as the bytes are equal and you stay inside the look-ahead.
        //     Try all candidate positions in the buffer; keep the longest match.

        // Append a back-reference token to the token array: { d, l }
        //     d = distance from position 256 back to where the match starts
        //         (i.e. d = 256 - match_position), range 1..255
        //     l = length of the matched sequence, range 1..255
        /* REVIEW: the token is stored as a packed uint16_t (distance << 8 | value),
         *         but the file format and Huffman stage expect the token stream to be
         *         a flat byte array where each token is two consecutive bytes:
         *         byte[0] = distance, byte[1] = value (length).
         *         Either change token_array to unsigned char* and append two bytes,
         *         or keep uint16_t but document clearly that the high byte is 'd'
         *         and the low byte is 'l', and unpack accordingly in Stage 2 and
         *         in decompress(). Be consistent throughout. */
        token_array[token_count++] = distance << 8 | value;

        // SHIFT THE WINDOW by the number of bytes consumed
        //     (1 for a literal, l for a back-reference).
        //     memmove(window, window + shift, 512 - shift)
        //     Content slides left; old look-ahead becomes new buffer history.
        unsigned char shift = (distance == 0) ? 1 : value;
        memmove(window, window + shift, WINDOW_SIZE - shift);

        /* REVIEW: remove the line below. 'base' is a fixed boundary (always 256 /
         *         LOOK_AHEAD). The sliding is done by memmove on the window content,
         *         not by moving the pointer. Decrementing base here causes the search
         *         range to shrink incorrectly on every iteration. */
        base = base < shift ? 0 : base - shift;
        ahead -= shift;

        // REFILL the look-ahead after the shift.
        //     Read up to 'shift' new bytes from the input file into
        //     positions (512 - shift) .. 511.
        //     Advance 'ahead' only for bytes actually read.
        bytes_read = fread(window + (WINDOW_SIZE - shift), 1, shift, input_file);
        ahead += bytes_read;
    }

    // Close the input file.
    fclose(input_file);


    /* ---- STAGE 2 : HUFFMAN  --------------------------------------------- */

    /* Step 8 — BUILD FREQUENCY TABLE.
     *           Call build_freq_table(tokens, token_count, freq)
     *           to count how often each byte value appears in the token array. */

    /* Step 9 — BUILD HUFFMAN TREE.
     *           Call build_huffman_tree(freq) to get the root node.
     *           Call generate_codes(root, codes) to get the bit-code for every symbol. */

    /* Step 10 — OPEN OUTPUT FILE for writing in binary mode ("wb").
     *            Check that it opened successfully. */

    /* Step 11 — WRITE FILE HEADER (see format table at the top of this file):
     *              a. Magic bytes: 0x5A 0x49 0x50 0x43
     *              b. Version:     0x01
     *              c. ORIG_SIZE:   the original file size as uint64_t little-endian
     *              d. ORIG_NAME_LEN + ORIG_NAME
     *              e. HUFF_ENTRIES (uint16_t): number of symbols with freq > 0
     *              f. HUFF_TABLE:  for each such symbol write { symbol(1 byte), freq(4 bytes LE) }
     *              g. PADDING_BITS placeholder: write a 0x00 byte now;
     *                 you will overwrite it after encoding when you know the real value. */

    /* Step 12 — ENCODE the token array into a bit-stream using the Huffman codes.
     *            For each byte in tokens[]:
     *              look up codes[byte], then call write_bit() for each bit in its code.
     *            Write completed bytes to the output file as they fill up.
     *            Track the final bit_pos to compute PADDING_BITS = (8 - bit_pos) % 8. */

    /* Step 13 — FLUSH the last partial byte (if bit_pos > 0, write it padded with 0s). */

    /* Step 14 — Go back and overwrite the PADDING_BITS placeholder byte in the header
     *            with the real padding value (use fseek + fwrite). */

    /* Step 15 — Free the token array, free the Huffman tree.
     *            Close the output file. */
}

/* -----------------------------------------------------------------------
 * decompress
 *   TWO-STAGE DECOMPRESSION: Huffman decoding  →  LZ77 replay.
 *
 *   The compressed stream is a sequence of 2-byte LZ77 tokens:
 *     { 0,   byte }  -> literal: copy 'byte' directly to output
 *     { d,   l    }  -> back-reference: go back 'd' bytes from position 256
 *                       in the sliding window and copy 'l' bytes to output
 *
 * ----------------------------------------------------------------------- */
void decompress(const char *input_path, const char *output_path)
{
    /* ---- STAGE 1 : READ HEADER & REBUILD HUFFMAN TREE  ----------------- */

    /* Step 1 — Open input_path for reading in binary mode ("rb").
     *           Check that it opened successfully. */

    /* Step 2 — READ AND VALIDATE THE HEADER:
     *              a. Read and verify the 4 magic bytes (abort if wrong).
     *              b. Read and verify the version byte.
     *              c. Read ORIG_SIZE (uint64_t LE).
     *              d. Read ORIG_NAME_LEN, then read ORIG_NAME into a buffer.
     *              e. Read HUFF_ENTRIES (uint16_t LE).
     *              f. For each entry read { symbol(1 byte), freq(4 bytes LE) }
     *                 and rebuild the freq[256] table.
     *              g. Read PADDING_BITS (1 byte). */

    /* Step 3 — REBUILD THE HUFFMAN TREE from the freq table.
     *           Call build_huffman_tree(freq) to get the root.
     *           (No need to call generate_codes — you will walk the tree directly.) */

    /* Step 4 — READ the remaining compressed bytes into a memory buffer.
     *           You now know the byte count from the file position vs file size. */


    /* ---- STAGE 2 : HUFFMAN DECODE  -------------------------------------- */

    /* Step 5 — Declare a dynamic byte array to receive the decoded LZ77 tokens. */

    /* Step 6 — DECODE the bit-stream:
     *           Use a BitReader starting at the first compressed byte.
     *           Walk the Huffman tree bit by bit (0 → go left, 1 → go right).
     *           When a leaf is reached, append leaf->symbol to the token array
     *           and restart from the root.
     *           Stop when you have decoded enough bytes
     *           (track count vs expected token bytes, or detect the end of the
     *           bit-stream using the total bit count minus PADDING_BITS). */


    /* ---- STAGE 3 : LZ77 REPLAY  ---------------------------------------- */

    /* Step 7 — Declare the 512-byte sliding window array (same layout as compress).
     *           Declare 'wp' = 256 as the write pointer into the window. */

    /* Step 8 — Open output_path for writing in binary mode ("wb").
     *           Check that it opened successfully. */

    /* Step 9 — MAIN LZ77 REPLAY LOOP.
     *           Read the token array two bytes at a time: (d, l). */

        /* Step 9a — LITERAL TOKEN  (d == 0):
         *            Write the single byte 'l' to the output file.
         *            Place 'l' into window[255] (end of buffer)
         *            and shift the window left by 1 to absorb it into history. */

        /* Step 9b — BACK-REFERENCE TOKEN  (d > 0):
         *            Compute match_pos = 256 - d.
         *            Copy 'l' bytes starting at window[match_pos] to the output file.
         *            Shift the window left by 'l' to absorb those bytes into history. */

    /* Step 10 — Free the token array, free the Huffman tree.
     *            Close both files. */
}

/* -----------------------------------------------------------------------
 * main
 *   Entry point. Parses -c / -d flags and dispatches accordingly.
 * ----------------------------------------------------------------------- */
int main(int argc, char *argv[])
{
    /* We need exactly 4 arguments: program, flag, input, output */
    if (argc != 4) {
        usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-c") == 0) {
        compress(argv[2], argv[3]);
    } else if (strcmp(argv[1], "-d") == 0) {
        decompress(argv[2], argv[3]);
    } else {
        usage(argv[0]);
        return 1;
    }

    return 0;
}
