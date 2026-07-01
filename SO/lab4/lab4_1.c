#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    /* cria segmento de memória compartilhada para 1 inteiro */
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        exit(1);
    }

    /* mapeia o segmento no espaço de endereçamento do processo */
    int *n = (int *) shmat(shmid, NULL, 0);
    if (n == (int *) -1) {
        exit(1);
    }

    *n = 1;

    pid_t pid1, pid2, pid3;

    /* 3 processos irmãos (todos filhos do pai) */

    pid1 = fork();
    if (pid1 == 0) {
        for (int i = 0; i < 1000; i++)
            *n += 1;
        printf("processo filho1, pid=%d, n=%d\n", getpid(), *n);
        shmdt(n);
        exit(0);
    }

    pid2 = fork();
    if (pid2 == 0) {
        for (int i = 0; i < 1000; i++)
            *n += 10;
        printf("processo filho2, pid=%d, n=%d\n", getpid(), *n);
        shmdt(n);
        exit(0);
    }

    pid3 = fork();
    if (pid3 == 0) {
        for (int i = 0; i < 1000; i++)
            *n += 100;
        printf("processo filho3, pid=%d, n=%d\n", getpid(), *n);
        shmdt(n);
        exit(0);
    }

    /* pai aguarda todos os filhos terminarem */
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);

    /* desanexa e remove o segmento compartilhado */
    shmdt(n);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
