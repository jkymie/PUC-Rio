# Relatório – Trabalho 2: Compactador e Descompactador de Arquivos Texto

**Disciplina:** Estruturas de Dados e Algoritmos  
**Aluna:** Julia Kymie  
**Arquivo compactado/descompactado:** `textoCompactadorDescompactador.txt`

---

## 1. Descrição do Problema

O trabalho consiste em implementar um compactador e um descompactador de arquivos texto utilizando o **algoritmo de Huffman**. O princípio do algoritmo é atribuir códigos binários mais curtos aos caracteres mais frequentes e códigos mais longos aos menos frequentes, reduzindo o tamanho total do arquivo sem perda de informação.

---

## 2. Estruturas de Dados Utilizadas

### 2.1 Nó da Árvore de Huffman

```c
typedef struct Node {
    unsigned char ch;
    unsigned long freq;
    struct Node *left, *right;
} Node;
```

Cada nó armazena o caractere (`ch`), sua frequência (`freq`) e ponteiros para os filhos esquerdo e direito. Nós internos da árvore têm `ch = 0` e servem apenas para organizar a hierarquia.

### 2.2 Min-Heap (Fila de Prioridade)

```c
typedef struct {
    Node **arr;
    int size, capacity;
} MinHeap;
```

O min-heap é a estrutura central para a construção da árvore. Ele mantém os nós ordenados pelo menor valor de frequência, permitindo extrair sempre os dois nós de menor frequência em O(log n). As operações implementadas são:

- `insertHeap` — insere e sobe o nó na heap (`heapifyUp`)
- `extractMin` — remove a raiz e reorganiza a heap (`heapifyDown`)

### 2.3 Tabela de Códigos

```c
typedef struct {
    unsigned char ch;
    char code[MAX_CODE_LEN];
    int len;
} CodeEntry;
```

Armazena o código binário (como string de `'0'` e `'1'`) e o comprimento em bits de cada caractere presente no texto.

---

## 3. Algoritmo Implementado

### 3.1 Construção da Árvore de Huffman

1. Percorrer o arquivo e contar a frequência de cada byte.
2. Para cada caractere com frequência > 0, criar um nó folha e inserir no min-heap.
3. Repetir até restar um único nó no heap:
   - Extrair os dois nós de menor frequência (`left` e `right`).
   - Criar um nó pai com `freq = left->freq + right->freq`.
   - Inserir o nó pai no heap.
4. O nó restante é a raiz da árvore de Huffman.

### 3.2 Geração dos Códigos

Percurso DFS (profundidade) na árvore:
- Ao descer para o filho esquerdo, adiciona `'0'` ao código atual.
- Ao descer para o filho direito, adiciona `'1'`.
- Ao atingir uma folha, registra o código na tabela.

### 3.3 Compressão

O arquivo comprimido possui o seguinte formato:

| Campo | Tamanho | Descrição |
|---|---|---|
| `unique` | 4 bytes | Número de caracteres únicos |
| `(char, freq)` × unique | 9 bytes cada | Tabela de frequências para reconstrução |
| `totalBits` | 8 bytes | Total de bits no stream comprimido |
| stream de bits | variável | Bits empacotados em bytes |

Os bits são empacotados da esquerda para a direita dentro de cada byte (MSB first). Bits de padding no último byte são ignorados graças ao campo `totalBits`.

### 3.4 Descompressão

1. Ler o cabeçalho e reconstruir a tabela de frequências.
2. Reconstruir a árvore de Huffman com a mesma lógica da compressão.
3. Percorrer o stream de bits: a cada bit `0` vai para o filho esquerdo, `1` para o direito; ao atingir uma folha, emite o caractere e volta à raiz.
4. Parar quando `bitsRead == totalBits`.

---

## 4. Tabela de Frequências

Resultado obtido sobre o arquivo `textoCompactadorDescompactador.txt` (4740 caracteres):

| Caractere | Frequência |
|---|---|
| `\n` | 188 |
| `\r` | 188 |
| ` ` (espaço) | 799 |
| `A` | 306 |
| `B` | 41 |
| `C` | 84 |
| `D` | 144 |
| `E` | 383 |
| `F` | 78 |
| `G` | 52 |
| `H` | 175 |
| `I` | 227 |
| `J` | 1 |
| `K` | 124 |
| `L` | 161 |
| `M` | 64 |
| `N` | 302 |
| `O` | 370 |
| `P` | 31 |
| `R` | 171 |
| `S` | 160 |
| `T` | 283 |
| `U` | 117 |
| `V` | 39 |
| `W` | 105 |
| `Y` | 146 |
| `Z` | 1 |
| **TOTAL** | **4740** |

---

## 5. Tabela de Códigos Huffman

| Caractere | Código Binário | Bits | Frequência |
|---|---|---|---|
| `E` | `000` | 3 | 383 |
| `B` | `001000` | 6 | 41 |
| `G` | `001001` | 6 | 52 |
| `W` | `00101` | 5 | 105 |
| `I` | `0011` | 4 | 227 |
| `U` | `01000` | 5 | 117 |
| `K` | `01001` | 5 | 124 |
| `M` | `010100` | 6 | 64 |
| `J` | `010101000` | 9 | 1 |
| `Z` | `010101001` | 9 | 1 |
| `P` | `01010101` | 8 | 31 |
| `V` | `0101011` | 7 | 39 |
| `D` | `01011` | 5 | 144 |
| `T` | `0110` | 4 | 283 |
| `N` | `0111` | 4 | 302 |
| `A` | `1000` | 4 | 306 |
| `Y` | `10010` | 5 | 146 |
| `S` | `10011` | 5 | 160 |
| `L` | `10100` | 5 | 161 |
| `F` | `101010` | 6 | 78 |
| `C` | `101011` | 6 | 84 |
| `R` | `10110` | 5 | 171 |
| `H` | `10111` | 5 | 175 |
| `O` | `1100` | 4 | 370 |
| `\n` | `11010` | 5 | 188 |
| `\r` | `11011` | 5 | 188 |
| ` ` (espaço) | `111` | 3 | 799 |

Os caracteres mais frequentes (`' '`, `'E'`, `'O'`) recebem os códigos mais curtos (3 bits), enquanto os menos frequentes (`'J'`, `'Z'`) recebem os mais longos (9 bits), confirmando o princípio do algoritmo.

---

## 6. Estatísticas de Compressão

| Métrica | Valor |
|---|---|
| Tamanho original | 4740 bytes (37920 bits) |
| Bits de dados comprimidos | 20346 bits |
| Bytes de dados comprimidos | 2544 bytes |
| Tamanho do arquivo `.huf` (com cabeçalho) | 2799 bytes |
| Taxa de compressão (dados) | **46,34%** |

O arquivo original ocupa 4740 bytes. Após a compressão, o stream de bits dos dados representa apenas 20346 bits (≈ 2544 bytes), uma redução de 46,34% em relação ao tamanho original. O arquivo `.huf` final tem 2799 bytes pois inclui o cabeçalho com a tabela de frequências necessária para a descompressão.

---

## 7. Verificação

Após descompactar o arquivo `textoComprimido.huf` para `textoDescomprimido.txt`, foi realizada uma comparação byte a byte com o arquivo original:

```
OK: arquivo descomprimido identico ao original.
```

O algoritmo não tem perda de informação.

---

## 8. Complexidade

| Operação | Complexidade |
|---|---|
| Contagem de frequências | O(n) |
| Construção da heap inicial | O(k log k) |
| Construção da árvore | O(k log k) |
| Geração dos códigos | O(k) |
| Compressão | O(n log k) |
| Descompressão | O(n log k) |

Onde `n` é o número de caracteres no arquivo e `k` é o número de caracteres únicos (no máximo 256).

---

## 9. Como Compilar e Executar

```bash
gcc -Wall -O2 -o huffman huffman.c
./huffman textoCompactadorDescompactador.txt
```

Os arquivos gerados são:
- `textoComprimido.huf` — arquivo comprimido
- `textoDescomprimido.txt` — arquivo descomprimido (idêntico ao original)
