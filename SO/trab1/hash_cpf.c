#include <stdio.h>
#include <stdlib.h>

#define N      5003
#define HMOD2  4999
#define MAX    4096

long long tabela[N];

void inicializa() {
    int i;
    for (i = 0; i < N; i++)
        tabela[i] = -1;
}

int insere(long long cpf) {
    int pos   = (int)(cpf % N);
    int passo = (int)(1 + (cpf % HMOD2));
    int i;

    for (i = 0; i < N; i++) {
        if (tabela[pos] == -1 || tabela[pos] == cpf) {
            tabela[pos] = cpf;
            return i;
        }
        pos = (pos + passo) % N;
    }
    return -1;
}

int main() {
    long long cpfs[MAX];
    int n = 0;
    long long cpf;
    long long total = 0;
    int i, col;

    FILE *arq = fopen("CPFsValidos.txt", "r");
    if (!arq) {
        printf("erro: nao abriu CPFsValidos.txt\n");
        return 1;
    }

    while (n < MAX && fscanf(arq, "%lld", &cpf) == 1)
        cpfs[n++] = cpf;
    fclose(arq);

    printf("CPFs lidos: %d  tamanho da tabela: %d\n", n, N);

    FILE *csv = fopen("resultados.csv", "w");
    if (!csv) {
        printf("erro: nao foi possivel criar resultados.csv\n");
        return 1;
    }

    inicializa();
    fprintf(csv, "chaves_inseridas,colisoes_acumuladas\n");

    for (i = 0; i < n; i++) {
        col = insere(cpfs[i]);
        if (col > 0)
            total += col;

        if ((i + 1) % 100 == 0 || i + 1 == n)
            fprintf(csv, "%d,%lld\n", i + 1, total);
    }

    fclose(csv);
    printf("CSV gerado: resultados.csv\n");

    return 0;
}
