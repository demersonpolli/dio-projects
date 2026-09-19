# 2026 IBM Bob

Projeto de teste das funcionalidades do **IBM Bob**, usando como caso de uso
a implementação de um compressor de arquivos em C que combina **LZ77** e
**codificação de Huffman**.

## Sobre o projeto

O arquivo [`compressor.c`](compressor.c) implementa um compressor/descompressor
de arquivos em dois estágios:

1. **LZ77** — uma janela deslizante de 512 bytes (256 de histórico + 256 de
   *look-ahead*) é usada para substituir sequências de bytes repetidas por
   referências `(distância, tamanho)`.
2. **Huffman** — o fluxo de tokens gerado pelo LZ77 é recodificado em bits
   usando códigos de tamanho variável baseados na frequência de cada byte,
   reduzindo ainda mais o tamanho final.

O formato de arquivo comprimido resultante é o `.zipc`, com o seguinte layout
de cabeçalho:

| Offset   | Tamanho | Campo             | Descrição                                              |
|----------|---------|-------------------|----------------------------------------------------------|
| 0        | 4       | `MAGIC`           | `0x5A 0x49 0x50 0x43` ("ZIPC")                            |
| 4        | 1       | `VERSION`         | `0x01`                                                    |
| 5        | 8       | `ORIG_SIZE`       | tamanho do arquivo original (uint64 LE)                   |
| 13       | N+1     | `ORIG_NAME`       | nome do arquivo original (com terminador nulo)            |
| 14+N     | 2       | `HUFF_ENTRIES`    | número de entradas na tabela de Huffman (uint16 LE)       |
| 16+N     | 5*E     | `HUFF_TABLE`      | E entradas `{ symbol(uint16 LE), freq(uint32 LE) }`       |
| 16+N+5E  | 8       | `DATA_BYTES`      | tamanho do bloco de dados comprimido (uint64 LE)          |
| 24+N+5E  | 1       | `PADDING_BITS`    | bits de padding no último byte (0..7)                     |
| 25+N+5E  | *       | `COMPRESSED_DATA` | fluxo de tokens LZ77 codificado em Huffman                |

## Status do projeto

Este é um projeto **experimental / em desenvolvimento**, usado para testar o
IBM Bob em um exercício de revisão e continuação de código C. Nem todo o
pipeline está funcional ainda:

- O estágio **LZ77** de `compress()` está implementado e já teve dois bugs
  corrigidos (a variável `count` agora é reiniciada a cada novo candidato de
  correspondência, e a comparação de "melhor correspondência" passou a usar
  `count > best_len` corretamente). Ainda restam bugs conhecidos, sinalizados
  com comentários `TODO:` no código:
  - ao emitir um literal (`distance == 0`), o token gravado deveria ser o
    byte bruto (`window[LOOK_AHEAD]`), mas o código atual grava
    `(distance << 8) | best_len`, que resulta em `0` para qualquer literal;
  - a variável `base` continua sendo indevidamente decrementada durante o
    laço principal, quando deveria permanecer fixa em `256` — isso encolhe
    a janela de busca a cada iteração até restarem apenas literais.
- O estágio **Huffman** ganhou funções auxiliares completas
  (`build_freq_table`, `build_huffman_queue`, `write_bit`/`read_bit` com
  `BitWriter`/`BitReader`), mas partes centrais ainda não estão prontas:
  - `build_huffman_tree()` tem três bugs documentados via `TODO`: a lógica de
    inserção/`memmove` no array de prioridade está incorreta, o campo
    `leaf->num_symbols` nunca é atribuído ao mesclar dois nós, e o array
    `leaf->symbols` é alocado mas nunca preenchido com os símbolos dos
    filhos;
  - `traverse_tree()` e `generate_codes()` estão **stubbed**, com apenas
    comentários `TODO 1`–`TODO 8` descrevendo os passos (caso base, checagem
    de folha, recursão esquerda/direita, alocação do buffer de bits);
  - `build_huffman_queue()` tem um `TODO` sobre vazamento de memória em caso
    de falha parcial de alocação (os nós já alocados não são liberados antes
    de retornar).
  - As 11 etapas de `compress()` que gravam o cabeçalho `.zipc` e codificam o
    stream de tokens em bits (chamada das funções acima, escrita do header,
    `BitWriter`, patch de `DATA_BYTES`/`PADDING_BITS`) ainda estão apenas
    descritas em comentários `TODO S2-1` a `S2-11`, sem nenhuma implementação.
- A função `decompress()` está **inteiramente stubbed**: os passos esperados
  (leitura/validação do header, reconstrução da árvore de Huffman, decodificação
  bit a bit e replay do LZ77) estão descritos em comentários `TODO D1-*`,
  `D2-*` e `D3-*`, sem nenhuma implementação.
- A limitação antiga (`FIXME`) do campo `data_bytes` como `uint16_t` (máximo
  de 65535 bytes de saída comprimida) foi **corrigida** — o campo agora é
  `uint64_t`, tanto na struct `ZipcHeader` quanto no layout do arquivo `.zipc`.

## Como compilar

```bash
gcc -o compressor compressor.c
```

## Uso (previsto)

```bash
./compressor -c <arquivo_entrada> <arquivo_saida>   # Comprimir
./compressor -d <arquivo_entrada> <arquivo_saida>   # Descomprimir
```

> Nota: a compilação funciona, mas a compressão/descompressão completa ainda
> não produz um arquivo `.zipc` válido nem restaura o arquivo original,
> devido aos itens listados em "Status do projeto".

## Licença

MIT.
