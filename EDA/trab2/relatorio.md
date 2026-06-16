# Relatório – Trabalho 2: Compactador e Descompactador de Arquivos Texto

## 1. Identificação

| Campo | Informação |
|---|---|
| Aluna | Julia Kymie |
| Matrícula | [inserir matrícula] |

---

## 2. Objetivo

O trabalho consiste em implementar, em linguagem C, um programa capaz de **compactar e descompactar arquivos texto** utilizando o algoritmo de codificação de Huffman. A ideia central é analisar a frequência com que cada caractere aparece no arquivo e atribuir representações binárias menores aos caracteres mais comuns e representações maiores aos mais raros. Dessa forma, o arquivo resultante ocupa menos espaço do que a representação ASCII padrão de 8 bits por caractere, sem nenhuma perda de informação. O programa deve, ao final, verificar automaticamente se o arquivo descompactado é idêntico ao original.

---

## 3. Estrutura do Programa

O programa está contido em um único arquivo, `huffman.c`, e é composto pelas estruturas de dados e funções descritas a seguir.

### 3.1 Estruturas de Dados

**`Node`** — nó da árvore de Huffman. Armazena o caractere (`ch`), sua frequência (`freq`) e ponteiros para os filhos esquerdo (`esq`) e direito (`dir`). Nós internos (não-folha) têm `ch = 0` e existem apenas para organizar a hierarquia da árvore.

**`Codigo`** — entrada da tabela de códigos. Armazena o caractere (`ch`), seu código binário como string de `'0'` e `'1'` (`codigo`) e o comprimento em bits (`bits`).

**`tabelaCodigos[256]` / `totalCodigos`** — variáveis globais que formam a tabela de consulta rápida usada durante a compactação.

### 3.2 Funções

| Função | Descrição |
|---|---|
| `criarNo(ch, freq)` | Aloca e retorna um novo nó com os valores informados e filhos nulos. |
| `libera(raiz)` | Libera recursivamente toda a memória da árvore (pós-ordem). |
| `encontrarMinimo(nos[], n)` | Faz uma busca linear no vetor de nós ativos e retorna o índice do nó com menor frequência. |
| `construirArvore(freq[])` | Recebe o vetor de frequências e constrói a árvore de Huffman iterativamente, juntando os dois nós de menor frequência a cada passo. |
| `gerarCodigos(raiz, buf, profundidade)` | Percorre a árvore em profundidade (DFS): desce à esquerda adicionando `'0'` e à direita adicionando `'1'`; ao atingir uma folha, salva o código na tabela global. |
| `buscarCodigo(c)` | Busca linearmente na tabela global e retorna o código binário do caractere `c`. |
| `imprimirCaractere(c)` | Exibe um caractere de forma legível, tratando casos especiais como `'\n'`, `'\r'` e espaço. |
| `imprimirTabelaFreq(freq[], total)` | Imprime a tabela de frequências formatada no terminal. |
| `imprimirTabelaCodigos(freq[])` | Imprime a tabela de códigos Huffman gerados, com frequência e quantidade de bits. |
| `compactar(entrada, saida, freq[])` | Grava o arquivo `.huf`: escreve o cabeçalho (tabela de frequências + total de bits) e em seguida os bits dos dados empacotados em bytes. |
| `descompactar(entrada, saida)` | Lê o cabeçalho do `.huf`, reconstrói a árvore e percorre o stream de bits para reproduzir o arquivo original. |
| `verificar(original, descomprimido)` | Compara os dois arquivos byte a byte e informa se são idênticos. |
| `main(argc, argv)` | Ponto de entrada: lê o arquivo passado como argumento, conta as frequências e chama, em sequência, as funções de construção da árvore, geração de códigos, compactação, descompactação e verificação. |

---

## 4. Solução

### 4.1 Visão Geral do Fluxo

```
Arquivo original
      │
      ▼
[Contar frequências de cada byte]
      │
      ▼
[Construir Árvore de Huffman]
      │
      ▼
[Gerar tabela de códigos binários]
      │
      ├──► [Compactar → textoComprimido.huf]
      │           │
      │           ▼
      │    [Descompactar → textoDescomprimido.txt]
      │           │
      │           ▼
      └──────► [Verificar: original == descomprimido?]
```

### 4.2 Passo a Passo

**Passo 1 – Leitura e contagem de frequências**

O `main` abre o arquivo em modo binário e percorre todos os bytes com `fgetc`, incrementando `freq[(unsigned char)ch]++` para cada byte lido. Ao final, `freq[i]` contém quantas vezes o byte `i` aparece no arquivo.

**Passo 2 – Construção da Árvore de Huffman (`construirArvore`)**

Para cada byte `i` com `freq[i] > 0`, um nó folha é criado e inserido num vetor `nos[]`. Em seguida, o algoritmo repete o seguinte até restar apenas um nó:

1. Encontra o nó de menor frequência (`encontrarMinimo`) — índice `idxA` — e o remove do vetor.
2. Encontra o próximo nó de menor frequência — índice `idxB` — e o remove.
3. Cria um nó pai com `freq = a->freq + b->freq`, tendo `a` como filho esquerdo e `b` como filho direito.
4. Insere o nó pai de volta no vetor.

O único nó restante é a raiz da árvore.

> **Complexidade:** a busca linear em `encontrarMinimo` é O(k) e é chamada 2(k−1) vezes, onde k é o número de caracteres únicos. No pior caso (k = 256), isso resulta em O(k²) para a construção da árvore — adequado para o escopo do trabalho.

**Passo 3 – Geração dos códigos (`gerarCodigos`)**

A função percorre a árvore recursivamente. A cada chamada, `buf[profundidade]` recebe `'0'` na descida esquerda ou `'1'` na descida direita. Ao atingir uma folha (ambos os filhos nulos), o conteúdo acumulado em `buf` é salvo na tabela global `tabelaCodigos`.

**Passo 4 – Compactação (`compactar`)**

O arquivo `.huf` começa com um cabeçalho que contém o número de caracteres únicos, seguido pelos pares (byte, frequência) de cada um, e por fim o total de bits do stream comprimido. Essa informação é suficiente para reconstruir a árvore na descompactação sem precisar de nenhum arquivo auxiliar.

O programa percorre o arquivo de entrada duas vezes: na primeira passagem conta quantos bits serão necessários e grava esse valor no cabeçalho; na segunda passagem lê cada caractere, traduz para seu código Huffman e empacota os bits em bytes (8 bits por byte, do bit mais significativo para o menos significativo). O último byte é gravado mesmo incompleto — os bits de preenchimento são simplesmente ignorados durante a descompactação, pois o programa sabe exatamente quantos bits pertencem aos dados.

**Passo 5 – Descompactação (`descompactar`)**

O cabeçalho é lido para reconstruir `freq[]`, e a árvore é reconstruída com a mesma `construirArvore`. A seguir, o stream de bits é percorrido bit a bit: `0` navega para o filho esquerdo, `1` para o filho direito; ao atingir uma folha, o caractere é escrito no arquivo de saída e o ponteiro volta à raiz. O processo para exatamente quando `bitsLidos == totalBits`.

**Passo 6 – Verificação (`verificar`)**

Os dois arquivos são abertos e comparados byte a byte com `fgetc`. Se algum byte diferir, ou se os tamanhos forem distintos, a função reporta falha. Caso contrário, confirma que a compactação é sem perdas.

### 4.3 Saída do Programa

Executando com o arquivo `textoCompactadorDescompactador.txt` (letra de música em maiúsculas, 4 740 bytes):

```
$ gcc -Wall -O2 -o huffman huffman.c
$ ./huffman textoCompactadorDescompactador.txt

Total de caracteres: 4740

=== TABELA DE FREQUENCIAS ===
Caractere    Frequencia
----------   ----------
   '\n'               188
   '\r'               188
   ' '               799
   'A'               306
   'B'                41
   'C'                84
   'D'               144
   'E'               383
   'F'                78
   'G'                52
   'H'               175
   'I'               227
   'J'                 1
   'K'               124
   'L'               161
   'M'                64
   'N'               302
   'O'               370
   'P'                31
   'R'               171
   'S'               160
   'T'               283
   'U'               117
   'V'                39
   'W'               105
   'Y'               146
   'Z'                 1
   TOTAL           4740

=== TABELA DE CODIGOS HUFFMAN ===
Caractere    Codigo Binario             Bits         Freq
----------   ---------------           -----   ----------
   'E'        000                          3          383
   'B'        001000                       6           41
   'G'        001001                       6           52
   'W'        00101                        5          105
   'I'        0011                         4          227
   'U'        01000                        5          117
   'K'        01001                        5          124
   'M'        010100                       6           64
   'J'        010101000                    9            1
   'Z'        010101001                    9            1
   'P'        01010101                     8           31
   'V'        0101011                      7           39
   'D'        01011                        5          144
   'T'        0110                         4          283
   'N'        0111                         4          302
   'A'        1000                         4          306
   'Y'        10010                        5          146
   'S'        10011                        5          160
   'L'        10100                        5          161
   'F'        101010                       6           78
   'C'        101011                       6           84
   'R'        10110                        5          171
   'H'        10111                        5          175
   'O'        1100                         4          370
   '\n'        11010                        5          188
   '\r'        11011                        5          188
   ' '        111                          3          799

=== ESTATISTICAS DE COMPRESSAO ===
Tamanho original: 4740 bytes  (37920 bits)
Bits de dados: 20346 bits (2544 bytes arredondados)
Arquivo comprimido (.huf): 2687 bytes  (dados + cabecalho)
Arquivo comprimido salvo: textoComprimido.huf
Arquivo descomprimido salvo: textoDescomprimido.txt

=== VERIFICACAO
Deu certo!
```

Os caracteres mais frequentes (`' '` com 799 ocorrências, `'E'` com 383, `'O'` com 370) receberam os códigos mais curtos — 3 bits. Os menos frequentes (`'J'` e `'Z'`, 1 ocorrência cada) receberam os mais longos — 9 bits —, confirmando o princípio do algoritmo.

#### Teste com entrada simples (`ABRACADABRA!`)

Para facilitar a visualização da construção da árvore, foi testado também o arquivo `teste.txt` contendo apenas a string `ABRACADABRA!` (12 bytes):

```
$ ./huffman teste.txt

Total de caracteres: 12

=== TABELA DE FREQUENCIAS ===
Caractere    Frequencia
----------   ----------
   '!'                 1
   'A'                 5
   'B'                 2
   'C'                 1
   'D'                 1
   'R'                 2
   TOTAL             12

=== TABELA DE CODIGOS HUFFMAN ===
Caractere    Codigo Binario    Bits    Freq
----------   ---------------  -----  ------
   'A'        0                   1       5
   'D'        100                 3       1
   'R'        101                 3       2
   '!'        1100                4       1
   'C'        1101                4       1
   'B'        111                 3       2

=== ESTATISTICAS DE COMPRESSAO ===
Tamanho original: 12 bytes  (96 bits)
Bits de dados: 28 bits (4 bytes arredondados)
Arquivo comprimido (.huf): 42 bytes  (dados + cabecalho)
Arquivo comprimido salvo: textoComprimido.huf
Arquivo descomprimido salvo: textoDescomprimido.txt

=== VERIFICACAO
Deu certo!
```

`'A'`, o caractere mais frequente, recebe código de apenas 1 bit (`0`). O arquivo comprimido é maior que o original neste caso porque o cabeçalho com a tabela de frequências (42 bytes) domina o custo — situação esperada para entradas muito pequenas.

---

## 5. Observações e Conclusões

### 5.1 Como Compilar e Executar

```bash
gcc -Wall -O2 -o huffman huffman.c
./huffman <arquivo_de_entrada.txt>
```

Os arquivos gerados são salvos no mesmo diretório de trabalho:
- `textoComprimido.huf` — arquivo comprimido
- `textoDescomprimido.txt` — arquivo descomprimido (deve ser idêntico ao original)

### 5.2 O Que Funciona

- Compactação e descompactação de qualquer arquivo texto com bytes de 0 a 255.
- Cabeçalho auto-suficiente: o arquivo `.huf` carrega toda a informação necessária para a descompactação (tabela de frequências e total de bits), sem depender de arquivos auxiliares.
- Verificação automática da integridade após descompactação.
- Tratamento correto do último byte parcial por meio do campo `totalBits`.
- Impressão legível de caracteres especiais (`'\n'`, `'\r'`, `' '`) nas tabelas.

### 5.3 Limitações e Aspectos a Destacar

- **Busca linear no lugar de heap:** a função `encontrarMinimo` realiza uma busca O(k) a cada passo da construção da árvore, resultando em complexidade O(k²) total. Para 256 símbolos possíveis isso é irrelevante na prática, mas uma implementação com min-heap reduziria para O(k log k). A abordagem linear foi escolhida pela simplicidade de implementação.
- **Arquivo de entrada com um único caractere único:** se o arquivo contiver apenas um tipo de byte (ex.: `AAAA`), `construirArvore` retorna um único nó sem filhos. Nesse caso, `gerarCodigos` atribui o código vazio (`""`), e `buscarCodigo` retorna `""`. A compactação produzirá zero bits, e a descompactação entrará em loop infinito pois `atual->esq == NULL` mas o nó não é folha da forma esperada. Esse caso de borda não foi tratado no código atual.
- **Arquivos binários arbitrários:** o programa lida com qualquer byte (usa `unsigned char` e abre os arquivos em modo `"rb"`/`"wb"`), portanto funciona corretamente com arquivos texto tanto no Windows (CRLF) quanto no Linux (LF).
- **Variáveis globais:** `tabelaCodigos` e `totalCodigos` são variáveis globais, o que tornaria problemático o uso do programa como biblioteca. Para uma versão de produção, seria preferível passar a tabela por parâmetro.

### 5.4 Resultado dos Testes

| Arquivo de entrada | Tamanho original | Bits de dados | Arquivo `.huf` | Verificação |
|---|---|---|---|---|
| `textoCompactadorDescompactador.txt` | 4 740 bytes | 20 346 bits (~2 544 B) | 2 687 bytes | **Deu certo!** |
| `teste.txt` (`ABRACADABRA!`) | 12 bytes | 28 bits (~4 B) | 42 bytes | **Deu certo!** |

No arquivo maior, a compactação reduziu o stream de dados em **~46,3%** (de 37 920 para 20 346 bits). O arquivo `.huf` final (2 687 bytes) ainda representa uma redução de **43,3%** em relação ao original, mesmo incluindo o cabeçalho. No arquivo pequeno, o cabeçalho domina e o `.huf` fica maior — comportamento esperado e documentado.
