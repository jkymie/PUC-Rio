#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define NUM_PROCESSOS 10

int main(int argc, char *argv[]) {
    /* tamanho pedido pelo enunciado */
    int tam = 10000; 

    /* alocando espaço da memória para o vetor */
    int shmid = shmget(IPC_PRIVATE, tam * sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        exit(1);
    }

    /* anexando a memória compartilhada */
    int *v = (int *) shmat(shmid, NULL, 0);
    if (v == (int *) -1) {
        exit(1);
    }

    /* inicializa todas as posições com 1 como pedido */
    for (int i = 0; i < tam; i++)
        v[i] = 1;

    pid_t pids[NUM_PROCESSOS];

    /* cria 10 processos irmãos */
    for (int p = 0; p < NUM_PROCESSOS; p++) {
        pids[p] = fork();
        if (pids[p] < 0) {
            exit(1);
        }
        if (pids[p] == 0) {
            /* todos os filhos multiplicam por 2 e somam 2 em todas as posições do vetor */
            for (int i = 0; i < tam; i++)
                v[i] = v[i] * 2 + 2;
            printf("trabalhador %d pid=%d terminou\n", p, getpid());
            shmdt(v);
            exit(0);
        }
    }

    /* pai espera todos os filhos terminarem */
    for (int p = 0; p < NUM_PROCESSOS; p++)
        waitpid(pids[p], NULL, 0);

    /* verificação automática de como ficou o vetor */
    int todos_iguais = 1;
    int referencia = v[0];
    int vmin = v[0];
    int vmax = v[0];
    int diferentes  = 0;

    for (int i = 1; i < tam; i++) {
        if (v[i] != referencia) {
            todos_iguais = 0;
            diferentes++;
        }
        if (v[i] < vmin) vmin = v[i];
        if (v[i] > vmax) vmax = v[i];
    }

    printf("\n--- Verificação do vetor resultante ---\n");
    if (todos_iguais) {
        printf("Todas as posicoes sao iguais: %d\n", referencia);
    } else {
        printf("As posicoes NAO sao todas iguais! (valores diferentes encontrados)\n");
        printf("  Posicoes diferentes de v[0]: %d de %d\n", diferentes, tam);
        printf("  Valor minimo encontrado : %d\n", vmin);
        printf("  Valor maximo encontrado : %d\n", vmax);
        printf("  Valor esperado (correto): 3070\n");
    }

    /* desanexa e remove o segmento compartilhado */
    shmdt(v);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
