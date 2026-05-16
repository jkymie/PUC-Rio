#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Tabela hash com double hashing para CPFs de 11 dígitos.
 *
 * Tamanho N = 5003 (primo) → folga de ~22% sobre 4096 chaves.
 * h1(k) = k % N
 * h2(k) = 1 + (k % H2_PRIME)   (H2_PRIME = 4999, primo)
 * sondagem: pos(i) = (h1(k) + i * h2(k)) % N
 *
 * Double hashing foi escolhido por produzir menor agrupamento
 * que sondagem linear ou quadrática.
 */

#define N        5003
#define H2_PRIME 4999
#define MAX_CPFS 4096
#define STEP     100
#define EMPTY    -1LL

typedef long long ll;

ll table[N];

static inline int h1(ll k) { return (int)(k % N); }
static inline int h2(ll k) { return (int)(1 + (k % H2_PRIME)); }

void table_init(void) {
    for (int i = 0; i < N; i++) table[i] = EMPTY;
}

/*
 * Insere k na tabela.
 * Retorna o número de colisões sofridas (0 se posição inicial estava livre).
 * Retorna -1 se a tabela estiver cheia.
 */
int insert(ll k) {
    int pos  = h1(k);
    int step = h2(k);

    for (int i = 0; i < N; i++) {
        if (table[pos] == EMPTY || table[pos] == k) {
            table[pos] = k;
            return i; /* i é o número de colisões antes de encontrar slot */
        }
        pos = (pos + step) % N;
    }
    return -1;
}

int main(void) {
    ll  cpfs[MAX_CPFS];
    int total = 0;

    FILE *fp = fopen("CPFsValidos.txt", "r");
    if (!fp) {
        fprintf(stderr, "Erro: nao foi possivel abrir CPFsValidos.txt\n");
        return 1;
    }

    ll cpf;
    while (total < MAX_CPFS && fscanf(fp, "%lld", &cpf) == 1)
        cpfs[total++] = cpf;
    fclose(fp);

    fprintf(stderr, "CPFs lidos: %d | Tamanho da tabela: %d\n", total, N);

    /* saída CSV para o gráfico */
    printf("chaves_inseridas,colisoes_acumuladas\n");

    table_init();
    long long acc = 0;

    for (int i = 0; i < total; i++) {
        int c = insert(cpfs[i]);
        if (c > 0) acc += c;

        /* imprime a cada STEP inserções e no final */
        if ((i + 1) % STEP == 0 || i + 1 == total)
            printf("%d,%lld\n", i + 1, acc);
    }

    return 0;
}
