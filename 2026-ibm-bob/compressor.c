/*
 * MIT License
 *
 * Copyright (c) 2024 Demerson André Polli
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ===========================================================================
 *  compressor.c — LZ77 + Huffman file compressor / decompressor
 *
 *  Two-stage pipeline:
 *    COMPRESS:   raw bytes → [LZ77] → token stream → [Huffman] → bit stream
 *    DECOMPRESS: bit stream → [Huffman decode] → token stream → [LZ77 replay] → raw bytes
 *
 *  Output format: .zipc  (see FILE FORMAT table below)
 * ===========================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

/* ---------------------------------------------------------------------------
 * Constants
 * --------------------------------------------------------------------------- */
#define WINDOW_SIZE 512   /* total sliding-window size in bytes                */
#define LOOK_AHEAD  256   /* size of each half: buffer half and look-ahead half */
#define MIN(a,b)    ((a) < (b) ? (a) : (b))

#define TRUE  1
#define FALSE 0

/*
 * ===========================================================================
 *  FILE FORMAT  (.zipc)
 * ===========================================================================
 *  Offset      Size   Field
 *  ----------  -----  -------------------------------------------------------
 *   0           4     MAGIC          — 0x5A 0x49 0x50 0x43  ("ZIPC")
 *   4           1     VERSION        — 0x01
 *   5           8     ORIG_SIZE      — original file size in bytes (uint64_t LE)
 *  13           N+1   ORIG_NAME      — original filename (null-terminated)
 *  14+N         2     HUFF_ENTRIES   — number of entries in the Huffman table (uint16_t LE)
 *  16+N         5*E   HUFF_TABLE     — E entries of { symbol(uint16 LE), freq(uint32 LE) }
 *  16+N+5E      8     DATA_BYTES     — number of bytes in the compressed data block (uint64_t LE)
 *  24+N+5E      1     PADDING_BITS   — how many bits in the last byte are padding (0..7)
 *  25+N+5E      *     COMPRESSED_DATA — Huffman-encoded LZ77 token stream (packed bits)
 * ===========================================================================
 */

/* ---------------------------------------------------------------------------
 * Forward type declarations (structs reference each other)
 * --------------------------------------------------------------------------- */

/*
 * LZ77Token — one packed token produced by the LZ77 stage.
 *
 * Encoding (uint16_t):
 *   high byte — distance (0 = literal token; 1..255 = back-reference distance)
 *   low  byte — payload  (literal byte value  OR  match length)
 *
 * This packing must be used consistently when writing to token_array and when
 * unpacking in Stage 2 of compress() and in decompress().
 */
typedef uint16_t LZ77Token;

/*
 * HuffEntry — one row of the Huffman frequency table.
 *   symbol  : the LZ77Token value
 *   freq    : how many times it appears in the token stream
 */
struct HuffEntry {
    LZ77Token symbol;
    uint32_t  freq;
};
typedef struct HuffEntry HuffEntry;

/*
 * ZipcHeader — in-memory representation of the .zipc file header.
 *   Fields map 1-to-1 to the FILE FORMAT table above.
 *   huff_table and compressed_data are heap-allocated arrays.
 */
struct ZipcHeader {
    uint32_t   magic;           /* 0x5A495043 ("ZIPC")                         */
    uint8_t    version;         /* 0x01                                         */
    uint64_t   orig_size;       /* original file size in bytes (LE)             */
    char      *orig_name;       /* null-terminated original filename            */
    uint16_t   huff_entries;    /* number of entries in huff_table              */
    HuffEntry *huff_table;      /* heap array of huff_entries HuffEntry values  */
    uint64_t   data_bytes;      /* compressed data block size in bytes          */
    uint8_t    padding_bits;    /* number of padding bits in the last byte (0..7) */
    uint8_t   *compressed_data; /* heap array of data_bytes bytes               */
};
typedef struct ZipcHeader ZipcHeader;

/*
 * HuffNode — one node in the Huffman binary tree.
 *
 * Leaf nodes:  left == NULL && right == NULL
 *              symbols[0..num_symbols-1] holds the token value(s)
 * Internal nodes: left / right point to child subtrees
 *                 symbols holds the union of all descendant symbols
 *                 (used only during tree construction; not needed at decode time)
 */
struct HuffNode {
    LZ77Token       *symbols;      /* symbol value(s) — meaningful in leaves   */
    uint32_t         num_symbols;  /* number of entries in symbols[]            */
    unsigned int     freq;         /* frequency / combined weight               */
    struct HuffNode *left;         /* 0-branch child, NULL in a leaf            */
    struct HuffNode *right;        /* 1-branch child, NULL in a leaf            */
};
typedef struct HuffNode HuffNode;

/*
 * HuffCode — the variable-length bit-code assigned to one symbol.
 *   bits[]  : bit values (each element is 0 or 1), MSB first
 *   length  : number of valid entries in bits[]
 */
struct HuffCode {
    LZ77Token     symbol;
    unsigned char bits[32]; /* one bit per cell; supports trees up to 32 levels deep */
    unsigned char length;   /* number of valid bits                                  */
};
typedef struct HuffCode HuffCode;

/*
 * BitWriter — helper to pack individual bits into a heap-allocated byte buffer.
 *   buf      : heap-allocated output buffer; grown automatically via realloc
 *   capacity : current allocated size of buf in bytes
 *   byte_pos : index of the byte currently being filled (0-based)
 *   bit_pos  : next bit position within the current byte (0 = LSB .. 7 = MSB)
 *
 * Initialise before first use:
 *   writer.buf      = (unsigned char *)calloc(BITWRITER_INIT_CAP, 1);
 *   writer.capacity = BITWRITER_INIT_CAP;
 *   writer.byte_pos = 0;
 *   writer.bit_pos  = 0;
 */
#define BITWRITER_INIT_CAP 4096

struct BitWriter {
    unsigned char *buf;
    int            capacity;
    int            byte_pos;
    int            bit_pos;
};
typedef struct BitWriter BitWriter;

/*
 * BitReader — helper to unpack individual bits from a read-only byte buffer.
 *   buf      : pointer to the compressed data block (not owned)
 *   end_pos  : one-past-the-last valid byte index (== data_bytes)
 *   byte_pos : index of the byte currently being read (0-based)
 *   bit_pos  : next bit position within the current byte (0 = LSB .. 7 = MSB)
 *
 * The caller is responsible for never calling read_bit() more than
 * (end_pos * 8 - padding_bits) times in total.
 */
struct BitReader {
    const unsigned char *buf;
    int                  end_pos;
    int                  byte_pos;
    int                  bit_pos;
};
typedef struct BitReader BitReader;

/* ===========================================================================
 *  SORTING — quicksort for HuffEntry arrays
 * ===========================================================================
 *
 *  partition() and quicksort() sort a HuffEntry array in ascending order by
 *  the freq field.  Used by build_freq_table() before building the Huffman
 *  tree so that the lowest-frequency symbols are processed first.
 * =========================================================================== */

/*
 * partition — Lomuto partition scheme used by quicksort().
 *
 *   arr  : array to partition (in-place)
 *   low  : inclusive left bound
 *   high : inclusive right bound; arr[high] is used as the pivot
 *
 *   Returns the final index of the pivot after partitioning.
 */
int partition(HuffEntry *arr, int32_t low, int32_t high) {
    HuffEntry pivot = arr[high];
    int32_t i = low - 1;
    for (int32_t j = low; j < high; j++) {
        if (arr[j].freq < pivot.freq) {
            i++;
            HuffEntry temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    HuffEntry temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

/*
 * quicksort — recursive quicksort over HuffEntry[low..high] by freq (ascending).
 *
 *   arr  : array to sort (in-place)
 *   low  : inclusive left bound  (pass 0 for the full array)
 *   high : inclusive right bound (pass count-1 for the full array)
 *
 * Note: 'low' and 'high' are treated as int32_t to handle the case when low==0 correctly.
 */
void quicksort(HuffEntry *arr, int32_t low, int32_t high) {
    if (low < high) {
        int32_t pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

/* ===========================================================================
 *  HUFFMAN HELPER FUNCTIONS
 * =========================================================================== */

/*
 * build_freq_table — count how often each LZ77Token value appears in tokens[].
 *
 *   tokens : input array of LZ77Token values  (read-only)
 *   count  : on entry, the number of tokens in the array;
 *            on exit,  the number of distinct symbols found
 *
 *   Returns a heap-allocated HuffEntry array of (*count) entries sorted by
 *   frequency (ascending), or NULL on allocation failure.
 *   The caller is responsible for free()ing the returned array.
 */
HuffEntry *build_freq_table(LZ77Token *tokens, uint32_t *count) {
    uint32_t  entries_count = 0;
    char      found         = FALSE;
    uint32_t  total         = *count;
    LZ77Token entry;

    HuffEntry *all_tokens = (HuffEntry *)malloc(total * sizeof(HuffEntry));
    if (!all_tokens) return NULL;

    for (uint32_t idx = 0; idx < total; idx++) {
        entry = tokens[idx];
        found = FALSE;

        uint32_t pos = 0;
        for (pos = 0; pos < entries_count; pos++) {
            if (all_tokens[pos].symbol == entry) {
                found = TRUE;
                break;
            }
        }

        if (!found) {
            all_tokens[entries_count].symbol = entry;
            all_tokens[entries_count].freq   = 1;
            entries_count++;
        } else {
            all_tokens[pos].freq++;
        }
    }

    quicksort(all_tokens, 0, entries_count - 1);

    *count = entries_count;
    return all_tokens;
}

/*
 * build_huffman_queue — convert a sorted frequency table into an initial
 *                       priority queue of leaf HuffNodes.
 *
 *   freq_table : sorted HuffEntry array (ascending by freq) with *count entries
 *   count      : on entry, number of entries in freq_table;
 *                on exit,  number of successfully allocated nodes (may be less
 *                          than the input count if allocation fails mid-way)
 *
 *   Returns a heap-allocated array of (*count) HuffNode pointers, each pointing
 *   to a newly allocated leaf node, or NULL if the queue allocation fails.
 *
 * TODO (partial-failure leak) — When malloc fails for a node or its symbols
 *   array mid-loop, the function sets *count = idx and returns the partial queue.
 *   The already-allocated nodes (indices 0..idx-1) are NOT freed before
 *   returning.  The caller has no way to distinguish a partial result from a
 *   full one.  Fix: either free all allocated nodes before returning NULL, or
 *   document clearly that the caller must free the partial array using the
 *   updated *count value.
 */
HuffNode **build_huffman_queue(HuffEntry *freq_table, uint32_t *count) {
    HuffNode **queue = (HuffNode **)malloc(*count * sizeof(HuffNode *));
    if (!queue) return NULL;

    for (uint32_t idx = 0; idx < *count; idx++) {
        queue[idx] = (HuffNode *)malloc(sizeof(HuffNode));
        if (!queue[idx]) {
            *count = idx;
            return queue;
        }
        queue[idx]->symbols = (LZ77Token *)malloc(sizeof(LZ77Token));
        if (!queue[idx]->symbols) {
            free(queue[idx]);
            *count = idx;
            return queue;
        }

        *(queue[idx]->symbols) = freq_table[idx].symbol;
        queue[idx]->num_symbols = 1;
        queue[idx]->freq        = freq_table[idx].freq;
        queue[idx]->left        = NULL;
        queue[idx]->right       = NULL;
    }

    return queue;
}

/*
 * build_huffman_tree — merge the priority queue of leaf nodes into a single
 *                      Huffman binary tree using the greedy algorithm.
 *
 *   queue : array of HuffNode pointers sorted by freq (ascending); the array
 *           is modified in-place during construction
 *   count : initial number of nodes in the queue
 *
 *   Returns the root HuffNode*, or NULL if queue is NULL or empty.
 *
 * TODO (bug — memmove direction) — The current memmove shifts queue[1..count-1]
 *   left by one slot, overwriting queue[0].  But the new 'leaf' node was already
 *   placed somewhere in queue[1..count-2] before the shift.  After the shift,
 *   queue[0] ends up pointing to the old queue[1], NOT to the merged leaf.
 *   The priority-queue maintenance is therefore broken: the merged node may end
 *   up in the wrong position or be lost entirely.
 *   Fix: design the insertion and shift as a single consistent operation:
 *     1. Remove queue[0] and queue[1] (the two nodes just merged).
 *     2. Find the correct sorted position for 'leaf' in queue[2..count-1].
 *     3. memmove to open a slot at that position.
 *     4. Place 'leaf' there.
 *     5. Decrement count by 1 (two removed, one inserted).
 *
 * TODO (bug — leaf->num_symbols never set) — After computing total_symbols and
 *   allocating leaf->symbols, the assignment  leaf->num_symbols = total_symbols
 *   is missing.  Every internal node will have num_symbols == 0 (uninitialised),
 *   which corrupts generate_codes() and any traversal that relies on this field.
 *   Fix: add  leaf->num_symbols = total_symbols;  after the malloc.
 *
 * TODO (bug — leaf->symbols never populated) — The symbols array is malloc'd
 *   but the symbol values from left->symbols and right->symbols are never copied
 *   into it.  The array contains uninitialised bytes.
 *   Fix: after setting leaf->num_symbols, add:
 *     memcpy(leaf->symbols,
 *            left->symbols,
 *            left->num_symbols  * sizeof(LZ77Token));
 *     memcpy(leaf->symbols + left->num_symbols,
 *            right->symbols,
 *            right->num_symbols * sizeof(LZ77Token));
 */
HuffNode *build_huffman_tree(HuffNode **queue, uint32_t count) {
    if (!queue || count == 0) return NULL;

    while (count > 1) {
        HuffNode *leaf = (HuffNode *)malloc(sizeof(HuffNode));
        if (!leaf) break;

        HuffNode *left  = queue[0];
        HuffNode *right = queue[1];
        uint32_t  total_symbols = left->num_symbols + right->num_symbols;

        leaf->symbols = (LZ77Token *)malloc(total_symbols * sizeof(LZ77Token));
        if (!leaf->symbols) {
            free(leaf);
            break;
        }

        /* TODO: set leaf->num_symbols = total_symbols; (see doc above)          */
        /* TODO: memcpy symbols from left and right into leaf->symbols (see doc)  */

        leaf->freq  = left->freq + right->freq;
        leaf->left  = left;
        leaf->right = right;

        /* TODO (bug): fix the insertion + memmove logic described above.
         *   The block below is INCORRECT — replace it entirely.                  */
        uint8_t inserted = FALSE;
        for (int idx = 1; idx < (int)count - 1; idx++) {
            if (leaf->freq < queue[idx + 1]->freq) {
                queue[idx] = leaf;
                inserted   = TRUE;
                break;
            }
        }
        if (!inserted) {
            queue[count - 1] = leaf;
        }
        memmove(&queue[0], &queue[1], (count - 1) * sizeof(HuffNode *));
        /* END of block to replace */

        count--;
    }

    return queue[0];
}

/*
 * traverse_tree — recursive post-order walk of the Huffman tree that records
 *                 the bit-code for every leaf symbol into the codes[] array.
 *
 *   node      : current node (start the recursion with root)
 *   codes     : output array of HuffCode; must be pre-allocated with enough
 *               slots to hold one entry per distinct symbol
 *   code_idx  : pointer to the next free slot index in codes[]; incremented
 *               each time a leaf is written
 *   bits      : scratch buffer (size >= 32) accumulating the bit-path from
 *               root to the current node; each element is 0 or 1
 *   depth     : current depth == number of valid bits already in bits[]
 */
void traverse_tree(HuffNode *node, HuffCode *codes, uint32_t *code_idx,
                   unsigned char *bits, unsigned char depth)
{
    /* TODO 1 — BASE CASE: if node == NULL, return immediately.                  */

    /* TODO 2 — LEAF CHECK: if (node->left == NULL && node->right == NULL):
     *   For i = 0 .. node->num_symbols - 1:
     *     codes[*code_idx].symbol = node->symbols[i];
     *     memcpy(codes[*code_idx].bits, bits, depth * sizeof(unsigned char));
     *     codes[*code_idx].length = depth;
     *     (*code_idx)++;
     *   return;  (do not recurse further)                                        */

    /* TODO 3 — RECURSE LEFT (0-branch):
     *   bits[depth] = 0;
     *   traverse_tree(node->left,  codes, code_idx, bits, depth + 1);           */

    /* TODO 4 — RECURSE RIGHT (1-branch):
     *   bits[depth] = 1;
     *   traverse_tree(node->right, codes, code_idx, bits, depth + 1);           */
}

/*
 * generate_codes — allocate and populate a HuffCode array by traversing the
 *                  Huffman tree rooted at 'root'.
 *
 *   root  : root of the Huffman tree built by build_huffman_tree()
 *   count : number of distinct symbols (== number of leaf nodes)
 *
 *   Returns a heap-allocated HuffCode array of 'count' entries, or NULL on
 *   failure.  The caller is responsible for free()ing the returned array.
 */
HuffCode *generate_codes(HuffNode *root, uint32_t count) {
    HuffCode *codes = (HuffCode *)malloc(count * sizeof(HuffCode));
    if (!codes) return NULL;

    /* TODO 5 — GUARD: if (root == NULL) { free(codes); return NULL; }           */

    /* TODO 6 — SCRATCH BUFFER:
     *   unsigned char bits[32];
     *   (32 entries is enough for a tree up to 32 levels deep)                  */

    /* TODO 7 — TRAVERSE:
     *   uint32_t code_idx = 0;
     *   traverse_tree(root, codes, &code_idx, bits, 0);
     *   After the call, code_idx should equal 'count'.  If it does not, the
     *   tree and the count argument are out of sync — add an assertion or log. */

    /* TODO 8 — return codes;                                                     */
}

/*
 * free_huffman_tree — recursively free every HuffNode in post-order, including
 *                     each node's symbols array.
 *
 *   root : root of the tree to free; silently does nothing if NULL
 */
void free_huffman_tree(HuffNode *root) {
    if (root == NULL) return;
    free_huffman_tree(root->left);
    free_huffman_tree(root->right);
    free(root->symbols);
    free(root);
}

/*
 * write_bit — store one bit into the BitWriter's buffer.
 *
 *   writer : BitWriter initialised with buf/capacity/byte_pos/bit_pos
 *   bit    : the bit value to store (0 or 1)
 *
 *   The bit is ORed into buf[byte_pos] at position bit_pos (LSB = 0).
 *   When bit_pos reaches 8 the writer advances to the next byte.  If that
 *   byte would exceed the current capacity the buffer is doubled with realloc;
 *   the new bytes are zeroed so that unset bits read back as 0.
 *
 *   Returns 0 on success, -1 if realloc fails (the writer is left unchanged
 *   so the caller can detect and handle the error).
 */
int write_bit(BitWriter *writer, uint8_t bit) {
    if (bit) {
        writer->buf[writer->byte_pos] |= (unsigned char)(1 << writer->bit_pos);
    }

    writer->bit_pos++;
    if (writer->bit_pos == 8) {
        writer->bit_pos = 0;
        writer->byte_pos++;

        /* Grow the buffer when the next byte would exceed capacity */
        if (writer->byte_pos >= writer->capacity) {
            int new_cap = writer->capacity * 2;
            unsigned char *grown = (unsigned char *)realloc(writer->buf, new_cap);
            if (!grown) return -1;
            /* Zero the newly allocated region so unwritten bits read as 0 */
            memset(grown + writer->capacity, 0, new_cap - writer->capacity);
            writer->buf      = grown;
            writer->capacity = new_cap;
        }
    }

    return 0;
}

/*
 * read_bit — read one bit from the BitReader's buffer.
 *
 *   reader : BitReader initialised with buf/end_pos/byte_pos/bit_pos
 *
 *   Returns the bit value (0 or 1), or -1 if the read would go past end_pos.
 *   Advances bit_pos; when bit_pos reaches 8, resets it to 0 and increments
 *   byte_pos.
 *
 *   The caller should stop calling read_bit() once the total number of bits
 *   consumed equals (end_pos * 8 - padding_bits).  The -1 return acts as a
 *   secondary safety net against accidental over-reads.
 */
int read_bit(BitReader *reader) {
    if (reader->byte_pos >= reader->end_pos) return -1;

    int bit = (reader->buf[reader->byte_pos] >> reader->bit_pos) & 1;
    reader->bit_pos++;
    if (reader->bit_pos == 8) {
        reader->bit_pos = 0;
        reader->byte_pos++;
    }
    return bit;
}

/* ---------------------------------------------------------------------------
 * Forward declarations
 * --------------------------------------------------------------------------- */
void compress(const char *input_path, const char *output_path);
void decompress(const char *input_path, const char *output_path);

/*
 * usage — print the command-line synopsis and exit with status 1.
 *
 *   program_name : argv[0] passed from main()
 */
void usage(const char *program_name)
{
    printf("Usage:\n");
    printf("  %s -c <input_file> <output_file>   Compress a file\n",   program_name);
    printf("  %s -d <input_file> <output_file>   Decompress a file\n", program_name);
}

/*
 * compress — two-stage compression: LZ77 sliding window → Huffman encoding.
 *
 *   input_path  : path to the raw input file to compress
 *   output_path : path of the .zipc output file to create
 *
 *   WINDOW LAYOUT (WINDOW_SIZE = 512 bytes):
 *
 *     index:  [ 0 ........... 255 | 256 ........... 511 ]
 *                  BUFFER HALF          LOOK-AHEAD HALF
 *
 *   'base'  is a fixed boundary at index LOOK_AHEAD (256); it never changes.
 *   'ahead' starts at LOOK_AHEAD and advances right as bytes are read from file.
 *
 *   Each LZ77 token is a uint16_t packed as: (distance << 8) | payload
 *     distance == 0  →  literal:        payload == the raw byte value
 *     distance >  0  →  back-reference: payload == match length
 */
void compress(const char *input_path, const char *output_path)
{
    /* =========================================================================
     * STAGE 1 — LZ77 sliding-window tokenisation
     * ========================================================================= */

    unsigned char window[WINDOW_SIZE];
    unsigned int  base  = LOOK_AHEAD;   /* fixed boundary; must NOT be modified  */
    unsigned int  ahead = LOOK_AHEAD;   /* advances as the look-ahead fills up    */

    /* --- Open input file ----------------------------------------------------- */
    FILE *input_file = fopen(input_path, "rb");
    if (!input_file) {
        fprintf(stderr, "compress: failed to open input file: %s\n", input_path);
        exit(EXIT_FAILURE);
    }

    /* --- Measure file size --------------------------------------------------- */
    /* TODO (portability) — ftell() returns long, which is 32 bits on some
     *   platforms and returns -1 on error.  Replace with fseeko()/ftello()
     *   (off_t) for correct large-file support, and check for a -1 error.       */
    fseek(input_file, 0, SEEK_END);
    long original_file_size = ftell(input_file);
    rewind(input_file);

    /* --- Allocate token array ------------------------------------------------ */
    /* The worst case (all literals) produces one token per input byte,
     * so original_file_size slots is sufficient.                                 */
    LZ77Token *token_array = (LZ77Token *)malloc(sizeof(LZ77Token) * original_file_size);
    size_t     token_count = 0;
    if (!token_array) {
        fprintf(stderr, "compress: failed to allocate token array.\n");
        fclose(input_file);
        exit(EXIT_FAILURE);
    }

    /* --- Initial fill of the look-ahead half --------------------------------- */
    size_t bytes_read = fread(window + LOOK_AHEAD, 1, WINDOW_SIZE - ahead, input_file);
    ahead += bytes_read;

    /* --- Main LZ77 loop ------------------------------------------------------ */
    while (ahead > LOOK_AHEAD) {
        unsigned char distance = 0;
        unsigned char value    = window[LOOK_AHEAD]; /* first byte of look-ahead */
        unsigned char best_len = 0;
        char          match_found = FALSE;

        /* Search backward through the buffer half for the best match */
        for (int pos = LOOK_AHEAD - 1; pos >= (int)base; pos--) {
            if (window[pos] == value) {
                match_found = TRUE;

                /* TODO (bug — count not reset) — Declare and reset a local
                 *   'count' variable to 0 HERE, before the inner loop below,
                 *   so that each candidate position starts a fresh length count.
                 *   Without this reset, length from the previous candidate
                 *   accumulates, producing inflated match lengths.               */
                unsigned char count = 0;

                /* Count match length at this candidate position */
                for (int idx = 0;
                     idx < LOOK_AHEAD - pos &&
                     window[pos + idx] == window[LOOK_AHEAD + idx];
                     idx++) {
                    count++;
                }

                /* Keep the longest match found so far */
                if (distance == 0 || count > best_len) {
                    distance = LOOK_AHEAD - pos;
                    best_len = count;
                }
            }
        }

        /* Emit token: literal (distance==0) or back-reference (distance>0) */
        /* TODO (token encoding) — When distance == 0, the payload must be the
         *   raw literal byte (window[LOOK_AHEAD]), not best_len (which is 0).
         *   The current expression  distance << 8 | best_len  correctly stores 0
         *   in the high byte but also stores 0 in the low byte for a literal.
         *   Fix: emit  (uint16_t)window[LOOK_AHEAD]  as the literal token, i.e.:
         *     if (distance == 0)
         *         token_array[token_count++] = window[LOOK_AHEAD];
         *     else
         *         token_array[token_count++] = (distance << 8) | best_len;      */
        token_array[token_count++] = (distance << 8) | best_len;

        /* Shift window left by the number of bytes consumed */
        unsigned char shift = (distance == 0) ? 1 : best_len;
        memmove(window, window + shift, WINDOW_SIZE - shift);

        /* TODO (bug — base must NOT change) — Remove the line below.
         *   'base' is a fixed index (LOOK_AHEAD = 256).  The window content
         *   moves via memmove above; moving the index as well causes the search
         *   range to shrink by 'shift' on every iteration, eventually reaching
         *   zero and making LZ77 emit only literals.
         *   Delete:  base = base < shift ? 0 : base - shift;                    */
        base = base < shift ? 0 : base - shift;

        ahead -= shift;

        /* Refill look-ahead */
        bytes_read = fread(window + (WINDOW_SIZE - shift), 1, shift, input_file);
        ahead += bytes_read;
    }

    fclose(input_file);

    /* =========================================================================
     * STAGE 2 — Huffman encoding
     * ========================================================================= */

    /* TODO S2-1 — BUILD FREQUENCY TABLE:
     *   uint32_t freq_count = (uint32_t)token_count;
     *   HuffEntry *freq_table = build_freq_table(token_array, &freq_count);
     *   if (!freq_table) { free(token_array); exit(EXIT_FAILURE); }             */

    /* TODO S2-2 — BUILD HUFFMAN QUEUE AND TREE:
     *   uint32_t queue_count = freq_count;
     *   HuffNode **queue = build_huffman_queue(freq_table, &queue_count);
     *   if (!queue) { free(freq_table); free(token_array); exit(EXIT_FAILURE); }
     *   HuffNode *root = build_huffman_tree(queue, queue_count);
     *   free(queue);   (the queue array itself; nodes are now owned by the tree) */

    /* TODO S2-3 — GENERATE HUFFMAN CODES:
     *   HuffCode *codes = generate_codes(root, freq_count);
     *   if (!codes) { free_huffman_tree(root); free(freq_table);
     *                 free(token_array); exit(EXIT_FAILURE); }                  */

    /* TODO S2-4 — OPEN OUTPUT FILE:
     *   FILE *output_file = fopen(output_path, "wb");
     *   if (!output_file) { ... clean up and exit ... }                         */

    /* TODO S2-5 — WRITE FILE HEADER (see FILE FORMAT table at top of file):
     *   a. Magic bytes:  fwrite("\x5A\x49\x50\x43", 1, 4, output_file);
     *   b. Version:      fwrite("\x01", 1, 1, output_file);
     *   c. ORIG_SIZE:    write original_file_size as uint64_t little-endian
     *   d. ORIG_NAME:    extract base filename from input_path;
     *                    write the null-terminated string (strlen+1 bytes)
     *   e. HUFF_ENTRIES: write freq_count as uint16_t little-endian
     *   f. HUFF_TABLE:   for each entry in freq_table[0..freq_count-1]:
     *                      write symbol as uint16_t LE, then freq as uint32_t LE
     *   g. DATA_BYTES placeholder: write 8 zero bytes (uint64_t); will be
     *                      patched with fseek+fwrite after encoding is complete
     *   h. PADDING_BITS placeholder: write 0x00; will be patched after encoding */

    /* TODO S2-6 — INITIALISE BitWriter:
     *   BitWriter writer;
     *   writer.buf      = (unsigned char *)calloc(BITWRITER_INIT_CAP, 1);
     *   writer.capacity = BITWRITER_INIT_CAP;
     *   writer.byte_pos = 0;
     *   writer.bit_pos  = 0;
     *   if (!writer.buf) { ... clean up and exit ... }                          */

    /* TODO S2-7 — ENCODE token stream:
     *   For each token in token_array[0..token_count-1]:
     *     Search codes[] for the matching symbol.
     *     if (write_bit(&writer, codes[i].bits[j]) < 0) { ... handle OOM ... }
     *   Compute PADDING_BITS = (8 - writer.bit_pos) % 8.                        */

    /* TODO S2-8 — FLUSH last partial byte:
     *   If writer.bit_pos > 0, the current byte is partially filled with 0-bits
     *   as padding; advance byte_pos and write the byte to the output file.     */

    /* TODO S2-9 — WRITE compressed data block:
     *   fwrite(writer.buf, 1, writer.byte_pos, output_file);                   */

    /* TODO S2-10 — PATCH DATA_BYTES and PADDING_BITS placeholders:
     *   Seek to the DATA_BYTES offset in the output file and write the actual
     *   data_bytes (uint64_t LE), then write the actual padding_bits (uint8_t). */

    /* TODO S2-11 — FREE and CLOSE:
     *   free(token_array);
     *   free(freq_table);
     *   free(codes);
     *   free_huffman_tree(root);
     *   free(writer.buf);
     *   fclose(output_file);                                                     */
}

/*
 * decompress — two-stage decompression: Huffman decoding → LZ77 replay.
 *
 *   input_path  : path to the .zipc compressed file to read
 *   output_path : path of the output file to write
 *
 *   Token interpretation (matches compress()):
 *     high byte == 0  →  literal:        low byte is the raw byte value
 *     high byte >  0  →  back-reference: high byte = distance, low byte = length
 */
void decompress(const char *input_path, const char *output_path)
{
    /* =========================================================================
     * STAGE 1 — Read header and rebuild Huffman tree
     * ========================================================================= */

    /* TODO D1-1 — OPEN INPUT FILE:
     *   FILE *input_file = fopen(input_path, "rb");
     *   if (!input_file) { fprintf(stderr, ...); exit(EXIT_FAILURE); }          */

    /* TODO D1-2 — READ AND VALIDATE HEADER:
     *   a. Read 4 bytes; compare to magic 0x5A495043; abort if wrong.
     *   b. Read 1 byte version; abort if != 0x01.
     *   c. Read uint64_t orig_size (little-endian).
     *   d. Read the null-terminated ORIG_NAME string into a local buffer.
     *   e. Read uint16_t huff_entries (little-endian).
     *   f. For each of huff_entries entries, read:
     *        symbol (uint16_t LE) and freq (uint32_t LE);
     *        store into a local HuffEntry array.
     *   g. Read uint64_t data_bytes (little-endian).
     *   h. Read uint8_t padding_bits.                                            */

    /* TODO D1-3 — REBUILD HUFFMAN TREE:
     *   uint32_t queue_count = huff_entries;
     *   HuffNode **queue = build_huffman_queue(freq_table, &queue_count);
     *   HuffNode *root   = build_huffman_tree(queue, queue_count);
     *   free(queue);
     *   (No need to call generate_codes — tree will be walked bit-by-bit.)      */

    /* TODO D1-4 — READ COMPRESSED DATA BLOCK:
     *   Allocate a buffer of data_bytes bytes.
     *   fread the entire compressed block into it.                               */

    /* =========================================================================
     * STAGE 2 — Huffman decode: bit stream → LZ77 token stream
     * ========================================================================= */

    /* TODO D2-1 — ALLOCATE token output buffer:
     *   The decoded token count is not known in advance; allocate conservatively,
     *   e.g. data_bytes * 8 tokens (upper bound), or use a dynamic array.       */

    /* TODO D2-2 — INITIALISE BitReader:
     *   BitReader reader;
     *   reader.buf      = compressed_block;
     *   reader.end_pos  = (int)data_bytes;
     *   reader.byte_pos = 0;
     *   reader.bit_pos  = 0;
     *   uint64_t total_bits = data_bytes * 8 - padding_bits;
     *   uint64_t bits_read  = 0;                                                 */

    /* TODO D2-3 — DECODE loop:
     *   HuffNode *cur = root;
     *   while (bits_read < total_bits):
     *     int bit = read_bit(&reader);
     *     if (bit < 0) break;   // safety: past end of buffer
     *     bits_read++;
     *     cur = (bit == 0) ? cur->left : cur->right;
     *     if (cur->left == NULL && cur->right == NULL):   // leaf reached
     *       token_array[token_count++] = cur->symbols[0];
     *       cur = root;                                   // restart from root   */

    /* =========================================================================
     * STAGE 3 — LZ77 replay: token stream → raw bytes
     * ========================================================================= */

    /* TODO D3-1 — DECLARE sliding window:
     *   unsigned char window[WINDOW_SIZE];  (same layout as compress)
     *   memset(window, 0, sizeof(window));  (clear history)                     */

    /* TODO D3-2 — OPEN OUTPUT FILE:
     *   FILE *output_file = fopen(output_path, "wb");
     *   if (!output_file) { ... clean up and exit ... }                         */

    /* TODO D3-3 — MAIN LZ77 REPLAY LOOP:
     *   for (size_t i = 0; i < token_count; i++):
     *     uint8_t  dist = (token_array[i] >> 8) & 0xFF;
     *     uint8_t  pay  =  token_array[i]        & 0xFF;                        */

    /* TODO D3-4 — LITERAL TOKEN  (dist == 0):
     *   fwrite(&pay, 1, 1, output_file);
     *   Shift window left by 1:  memmove(window, window+1, WINDOW_SIZE-1);
     *   window[WINDOW_SIZE - 1] = pay;                                          */

    /* TODO D3-5 — BACK-REFERENCE TOKEN  (dist > 0):
     *   int match_pos = LOOK_AHEAD - dist;   // position in buffer half
     *   for (int j = 0; j < pay; j++):
     *     uint8_t byte = window[match_pos + j];
     *     fwrite(&byte, 1, 1, output_file);
     *     Shift window left by 1:  memmove(window, window+1, WINDOW_SIZE-1);
     *     window[WINDOW_SIZE - 1] = byte;                                       */

    /* TODO D3-6 — FREE and CLOSE:
     *   free(compressed_block);
     *   free(token_array);
     *   free_huffman_tree(root);
     *   fclose(input_file);
     *   fclose(output_file);                                                     */
}

/*
 * main — entry point; parse -c / -d flags and dispatch to compress/decompress.
 *
 *   argc : argument count (must be exactly 4)
 *   argv : argv[1] = "-c" or "-d"
 *          argv[2] = input  file path
 *          argv[3] = output file path
 */
int main(int argc, char *argv[])
{
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
