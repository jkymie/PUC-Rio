# Checklist — Simulador de Memória Virtual (`sim-virtual`)

> Trabalho 2 de Sistemas Operacionais — INF1316
> Objetivo: implementar em **C** um simulador de memória virtual com paginação e **4 algoritmos de substituição de páginas** (LRU, NRU, Relógio/Clock e Ótimo).

---

## 0. Regras gerais (não esquecer)

- [ ] Linguagem: **C** (arquivos `.c` e `.h`).
- [ ] O programa é uma **simulação** — nada é realmente lido/escrito em disco; apenas contabiliza-se o que *seria* feito.
- [ ] Executável deve se chamar **`sim-virtual`**.
- [ ] Endereços de entrada têm **32 bits** e estão em **hexadecimal**.
- [ ] Cabeçalho do código-fonte deve conter **nomes dos integrantes do grupo** (até 4) e indicar quem implementou/executou cada algoritmo.
- [ ] **Não** incluir os arquivos `.log` no pacote de entrega.
- [ ] Diferenciar sempre: **fato** (resultado da simulação) x **hipótese/suposição** (parâmetros assumidos, ex.: intervalo de reset do NRU).

---

## 1. Interface de linha de comando

O simulador recebe **exatamente 4 argumentos**, nesta ordem:

| # | Argumento | Valores aceitos | Observação |
|---|-----------|-----------------|------------|
| a | Algoritmo de substituição | `LRU` / `NRU` / `Relogio` / `Otimo` | Definir strings aceitas e tratar maiúsc./minúsc. |
| b | Arquivo de entrada | ex.: `arquivo.log` | Caminho do trace |
| c | Tamanho da página/quadro (KB) | `4` ou `8` | Só precisa suportar 4KB e 8KB |
| d | Tamanho total da memória física (MB) | `1`, `2` ou `4` | |

Invocação de exemplo:
```
sim-virtual LRU arquivo.log 8 2
```
→ LRU, entrada `arquivo.log`, página de 8KB, memória física de 2MB.

- [ ] Ler `argv`/`argc` e validar quantidade de argumentos.
- [ ] Validar que o algoritmo informado é um dos 4 suportados.
- [ ] Validar tamanho de página (4 ou 8) e de memória (1, 2 ou 4).
- [ ] Emitir mensagem de erro clara e uso correto se algum argumento for inválido.

---

## 2. Leitura do arquivo de entrada (`.log`)

- [ ] Cada linha tem: **endereço hexadecimal** + espaço + letra **`R`** (leitura) ou **`W`** (escrita).
- [ ] Ler cada linha com:
  ```c
  fscanf(file, "%x %c", &addr, &rw);  // addr é unsigned
  ```
- [ ] `addr` deve ser `unsigned` (32 bits).
- [ ] `W` → marca a página como **modificada** (bit M = 1).
- [ ] Ler até o fim do arquivo (EOF).

Exemplo de conteúdo:
```
0044e4f8 R
0044e500 R
2f6965a0 W
00062368 R
```

Arquivos de trace que serão fornecidos (executar em **todos**):
- [ ] `compilador.log` (compilador)
- [ ] `matriz.log` (programa científico)
- [ ] `compressor.log` (compressor de arquivos)
- [ ] `simulador.log` (simulador de partículas)

---

## 3. Cálculos e parâmetros derivados

- [ ] **`s` (bits de deslocamento)** = número de bits para representar o offset dentro da página, calculado **dinamicamente** a partir do tamanho de página:
  - 4KB → `s = 12` (2¹² = 4096)
  - 8KB → `s = 13` (2¹³ = 8192)
  - Fórmula: `s = log2(tamanho_pagina_em_bytes)`.
- [ ] **Índice da página** a partir do endereço lógico:
  ```c
  page = addr >> s;
  ```
- [ ] **Número de quadros (frames) físicos**:
  ```
  num_frames = memoria_fisica_em_bytes / tamanho_pagina_em_bytes;
  ```
  Ex.: 2MB / 8KB = 256 quadros.
- [ ] **Tabela de páginas**: como o índice de página pode ter até 19–20 bits (2¹⁹ p/ 8KB, 2²⁰ p/ 4KB), a tabela pode ter até ~1M entradas — grande, mas aceitável para simulação (só há 1 programa em execução). Sugestão: array indexado pelo número da página guardando o quadro (`-1` = não presente).

---

## 4. Estruturas de dados

### Estrutura do quadro físico (obrigatória)
Cada quadro deve conter **no mínimo**:
- [ ] Número da página que ocupa o quadro (ou "vazio").
- [ ] **Bit R** (referenciada).
- [ ] **Bit M** (modificada / "suja").
- [ ] **Instante do último acesso** (para LRU).
- [ ] Campos adicionais conforme o algoritmo (a critério do grupo).

### Contador de tempo
- [ ] Variável `time` iniciando em **0**, incrementada **a cada iteração/acesso** do simulador, usada para registrar o momento de cada acesso.

### Estruturas auxiliares por algoritmo
- [ ] **LRU**: usar o campo "instante do último acesso".
- [ ] **NRU**: bits R e M já cobrem; definir mecanismo de **reset periódico do bit R**.
- [ ] **Relógio/Clock**: lista/array **circular** de quadros + um **ponteiro (ponteiro do relógio)**.
- [ ] **Ótimo**: **pré-carregar toda a sequência de acessos** em memória (array) para poder "olhar o futuro".

---

## 5. Laço principal da simulação

Para cada acesso lido do arquivo:
- [ ] Incrementar `time`.
- [ ] Calcular `page = addr >> s`.
- [ ] Verificar se a página já está em algum quadro (**hit** ou **page fault**):
  - **Hit**: atualizar bit R = 1, atualizar último acesso = `time`, se acesso for `W` marcar M = 1.
  - **Page fault**:
    - [ ] Incrementar contador de **page faults**.
    - [ ] Se há **quadro livre** → carregar a página nele (sem substituição).
    - [ ] Se **todos os quadros ocupados** → acionar o **algoritmo de substituição** escolhido para escolher a vítima.
      - [ ] Se a página vítima estava **modificada (M=1)** → incrementar contador de **páginas sujas escritas** (escrita de volta ao disco — teórica).
      - [ ] Carregar a nova página no quadro liberado; ajustar R/M/último acesso.
- [ ] Manter a tabela de páginas consistente (mapear página → quadro; desmapear a vítima).

> **Regra crítica:** os algoritmos de substituição **só entram em ação** quando **todos os quadros estão ocupados** E ocorre um page fault.

---

## 6. Algoritmos de substituição (implementar os 4)

### 6.1 LRU — Least Recently Used
- [ ] Escolher como vítima o quadro com o **menor "instante de último acesso"** (o acessado há mais tempo).

### 6.2 NRU — Not Recently Used
- [ ] Classificar as páginas em 4 classes pelos bits (R, M):
  - Classe 0: R=0, M=0
  - Classe 1: R=0, M=1
  - Classe 2: R=1, M=0
  - Classe 3: R=1, M=1
- [ ] Escolher vítima da **menor classe não vazia** (0 → 3).
- [ ] Implementar **reset periódico do bit R** (a cada N acessos, zerar todos os R). **Documentar no relatório o valor de N assumido** (suposição do grupo).

### 6.3 Relógio / Clock
- [ ] Manter os quadros em **estrutura circular** com um **ponteiro**.
- [ ] No page fault, examinar o quadro apontado:
  - Se **R=0** → é a vítima; substituir e avançar ponteiro.
  - Se **R=1** → zerar R, avançar ponteiro e repetir.

### 6.4 Ótimo (Optimal / Belady)
- [ ] Substituir a página que será acessada **no futuro mais remoto** (ou que **não será mais acessada**).
- [ ] Requer conhecer os acessos futuros → **pré-ler todo o trace** em um array.
- [ ] Para cada página em memória, encontrar o **próximo índice de acesso futuro**; escolher a de maior distância (ou "nunca mais acessada").
- [ ] **Incluir o pseudo-código deste algoritmo no relatório.**

---

## 7. Estatísticas a coletar

- [ ] **Total de page faults** (páginas carregadas na memória).
- [ ] **Total de páginas sujas escritas** de volta ao disco.
  - [ ] **Atenção:** páginas sujas que ainda estejam na memória **no final da execução NÃO são contadas** (não precisam ser escritas).

---

## 8. Formato de saída (relatório curto em tela)

Ao processar o último acesso, imprimir:
- [ ] Os **4 parâmetros** usados (arquivo, tamanho da memória, tamanho da página, algoritmo).
- [ ] **Número de page faults**.
- [ ] **Número de páginas escritas** (sujas).

Exemplo de saída (valores fictícios):
```
Executando o simulador...
Arquivo de entrada: arquivo.log
Tamanho da memoria fisica: 2 MB
Tamanho das páginas: 8 KB
Algoritmo de substituição: LRU
Número de Faltas de Páginas: 520
Número de Páginas Escritas: 352
```

---

## 9. Matriz de simulações a executar

Executar **todas** as combinações:

- Algoritmos (4): LRU, NRU, Relógio, Ótimo
- Arquivos (4): compilador, matriz, compressor, simulador
- Tamanhos de página (2): 4KB, 8KB
- Tamanhos de memória (3): 1MB, 2MB, 4MB

→ **4 × 4 × 2 × 3 = 96 execuções** (todos os resultados vão para a tabela do relatório).

- [ ] Recomendado: criar um **script** (shell/Makefile) que roda todas as combinações e coleta os resultados automaticamente.

---

## 10. Entregáveis

- [ ] Código-fonte do simulador: arquivos `.c` e `.h`.
- [ ] Relatório em **`.pdf`**.
- [ ] **NÃO** incluir os arquivos `.log`.
- [ ] Cabeçalho com integrantes do grupo (até 4) e quem fez o quê.
- [ ] (Opcional/possível) preparar **apresentação/demonstração oral**.

---

## 11. Conteúdo obrigatório do relatório (peso alto na nota)

- [ ] **Resumo do projeto**: parágrafos descrevendo a estrutura geral do simulador e as **estruturas de dados relevantes** de cada algoritmo.
- [ ] **Análise de desempenho** dos algoritmos:
  - [ ] Comparar page-faults e escritas de página **com a memória física fixa e o tamanho de página variando**.
  - [ ] Comparar os resultados entre os diferentes **tamanhos de página**.
  - [ ] Comparar os **algoritmos** entre si e **justificar** os valores obtidos.
- [ ] **Tabela** com dados de cada simulação: algoritmo, tamanho da página, tamanho da memória, nº de page-faults, nº de páginas sujas.
- [ ] **Descrição do algoritmo Ótimo em pseudo-código** (e garantir que seus dados estão na tabela).

---

## 12. Checklist final antes de entregar

- [ ] Compila sem erros/warnings (testar com `-Wall`).
- [ ] Funciona para **4KB e 8KB** (cálculo de `s` dinâmico).
- [ ] Funciona para **1MB, 2MB e 4MB** (nº de quadros correto).
- [ ] Os 4 algoritmos produzem resultados coerentes (ex.: Ótimo deve ter ≤ page faults que os demais).
- [ ] Contadores de page fault e páginas sujas conferem com a regra do "final da execução".
- [ ] Saída no formato pedido.
- [ ] Relatório com todas as seções obrigatórias.
- [ ] Pacote sem os `.log`.
- [ ] Cabeçalho e autoria preenchidos.