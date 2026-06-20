# Lab 5 — Árvore B: Busca por Intervalo

**Disciplina:** Estruturas de Dados e Algoritmos  
**Aluna:** Julia Kymie  
**Data:** 09/06/2026

---

## Objetivo

Implementar e testar a operação de **busca por intervalo** em uma Árvore B, retornando todas as chaves `k` tais que `lim_inf < k < lim_sup`.

---

## Estrutura de Dados

A árvore B utilizada tem ordem máxima `MAX = 4` e mínima `MIN = 2`, ou seja, cada nó pode armazenar entre 2 e 4 chaves e possuir entre 3 e 5 filhos (exceto a raiz).

```c
#define MAX 4
#define MIN 2

typedef struct no t_no;
struct no {
    int ndesc;          // número de chaves no nó
    int chave[MAX];     // vetor de chaves
    t_no *ramo[MAX+1];  // vetor de ponteiros para filhos
};
```

### Árvore construída

A função `constroi_arvore()` monta a seguinte árvore B de 3 níveis:

```
                      [100]
                    /       \
          [50, 75]             [120, 200]
         /   |   \            /    |    \
   [10,40] [60,70] [80,90] [110,115] [130,135,140,170] [220,230,240,250]
```

| Nó     | Tipo          | Chaves                |
|--------|---------------|-----------------------|
| raiz   | interno (1 chave)  | 100              |
| n1     | interno (2 chaves) | 50, 75           |
| n2     | interno (2 chaves) | 120, 200         |
| f1     | folha (2 chaves)   | 10, 40           |
| f2     | folha (2 chaves)   | 60, 70           |
| f3     | folha (2 chaves)   | 80, 90           |
| f4     | folha (2 chaves)   | 110, 115         |
| f5     | folha (4 chaves)   | 130, 135, 140, 170 |
| f6     | folha (4 chaves)   | 220, 230, 240, 250 |

---

## Implementação

### Busca por intervalo — `intervalo()`

```c
void intervalo(t_no *arv, int lim_inf, int lim_sup) {
    if (arv == NULL) return;

    for (int i = 0; i < arv->ndesc; i++) {
        intervalo(arv->ramo[i], lim_inf, lim_sup);
        if (arv->chave[i] > lim_inf && arv->chave[i] < lim_sup)
            printf("%d\n", arv->chave[i]);
    }
    intervalo(arv->ramo[arv->ndesc], lim_inf, lim_sup);
}
```

**Estratégia:** percurso **in-order** recursivo. Para cada nó, visita o ramo filho `i`, depois verifica a chave `i`, e ao fim visita o último ramo (à direita da última chave). Isso garante que as chaves são visitadas em ordem crescente.

A condição de impressão é **estritamente aberta**: `lim_inf < chave < lim_sup`.

---

## Testes e Resultados

### Intervalo (5, 300)

Espera-se todas as chaves da árvore (5 e 300 são exclusivos, mas nenhuma chave os assume).

```
10, 40, 50, 60, 70, 75, 80, 90, 100, 110, 115, 120, 130, 135, 140, 170, 200, 220, 230, 240, 250
```

**Total:** 21 chaves — todas as chaves da árvore.

---

### Intervalo (50, 100)

Chaves maiores que 50 e menores que 100 (50 e 100 excluídos).

```
60, 70, 75, 80, 90
```

**Total:** 5 chaves. Os limites 50 e 100 foram corretamente excluídos.

---

### Intervalo (70, 135)

Chaves maiores que 70 e menores que 135 (70 e 135 excluídos).

```
75, 80, 90, 100, 110, 115, 120, 130
```

**Total:** 8 chaves. Os limites 70 e 135 foram corretamente excluídos.

---

## Análise de Complexidade

| Operação         | Complexidade     |
|------------------|------------------|
| Busca por intervalo | O(log n + k) |

Onde `n` é o número total de chaves e `k` é o número de chaves no resultado. O fator `log n` vem da altura da árvore B e `k` do custo de reportar cada chave encontrada.

---

## Conclusão

A implementação da busca por intervalo em Árvore B foi realizada com sucesso. O percurso in-order recursivo percorre toda a árvore em ordem crescente, verificando a condição de intervalo aberto para cada chave. Os três casos de teste produziram resultados corretos, confirmando que os limites são exclusivos e que todas as chaves no intervalo são encontradas.
