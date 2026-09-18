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

| Offset   | Tamanho | Campo             | Descrição                                         |
|----------|---------|-------------------|----------------------------------------------------|
| 0        | 4       | `MAGIC`           | `0x5A 0x49 0x50 0x43` ("ZIPC")                     |
| 4        | 1       | `VERSION`         | `0x01`                                             |
| 5        | 8       | `ORIG_SIZE`       | tamanho do arquivo original (uint64 LE)            |
| 13       | N+1     | `ORIG_NAME`       | nome do arquivo original (com terminador nulo)     |
| 14+N     | 2       | `HUFF_ENTRIES`    | número de entradas na tabela de Huffman (uint16 LE)|
| 16+N     | 5*E     | `HUFF_TABLE`      | E entradas `{ symbol(1B), freq(uint32 LE) }`       |
| 16+N+5E  | 2       | `DATA_BYTES`      | tamanho do bloco de dados comprimido (LE)          |
| 18+N+5E  | 1       | `PADDING_BITS`    | bits de padding no último byte (0..7)              |
| 19+N+5E  | *       | `COMPRESSED_DATA` | fluxo de tokens LZ77 codificado em Huffman         |

## Status do projeto

Este é um projeto **experimental / em desenvolvimento**, usado para testar o
IBM Bob em um exercício de revisão e continuação de código C. Nem todo o
pipeline está funcional ainda:

- O estágio **LZ77** de `compress()` está implementado, mas contém bugs
  conhecidos, sinalizados com comentários `REVIEW:` no código, por exemplo:
  - a variável `count` não é reiniciada antes de contar o tamanho de um novo
    candidato de correspondência, inflando o tamanho encontrado;
  - a comparação de "melhor correspondência" (`count > value`) usa a variável
    errada, já que `value` já foi sobrescrita com o tamanho do candidato
    anterior;
  - o token LZ77 é armazenado como um `uint16_t` empacotado
    (`distance << 8 | value`), mas o restante do pipeline espera um array de
    bytes plano — é preciso escolher uma representação e manter consistência;
  - a variável `base` é indevidamente decrementada durante o laço principal,
    quando deveria permanecer fixa em `256`.
- O estágio **Huffman** de `compress()` (construção da tabela de frequência,
  árvore de Huffman, escrita do cabeçalho `.zipc` e codificação em bits) está
  apenas descrito em comentários `Step N —`, ainda **não implementado**.
- `build_huffman_tree()` está incompleta: a lista ligada atual reutiliza os
  ponteiros `left`/`right` tanto para a fase de lista quanto para a árvore
  final, o que gera conflito (ver comentário `REVIEW:` acima da struct
  `HuffNode`). É necessário reescrever usando um array de ponteiros (ou
  min-heap) ordenado por frequência.
- A função `decompress()` está **inteiramente stubbed**: apenas os passos
  esperados estão descritos em comentários, sem nenhuma implementação.
- Há uma limitação conhecida (`FIXME`) no campo `data_bytes`, definido como
  `uint16_t`, o que limita a saída comprimida a 65535 bytes — deveria ser
  `uint64_t` para suportar arquivos de tamanho arbitrário.

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
