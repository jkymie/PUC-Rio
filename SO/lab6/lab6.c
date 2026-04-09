#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

/*
 * Escalonador de tempo real (simulação com SIGSTOP/SIGCONT)
 *
 * A cada minuto:
 *   P1 executa do segundo 5  ao 25  (duração 20s)
 *   P2 executa do segundo 45 ao 60  (duração 15s)
 *   P3 executa quando nem P1 nem P2 estão ativos (0-5 e 25-45)
 */

#define N 3

static pid_t pids[N];
static int   rodando[N] = {0, 0, 0};

/* Envia SIGCONT apenas se o processo está parado */
static void continua(int i) {
    if (!rodando[i]) {
        kill(pids[i], SIGCONT);
        rodando[i] = 1;
        printf("[escalonador] P%d CONT (pid=%d)\n", i + 1, pids[i]);
        fflush(stdout);
    }
}

/* Envia SIGSTOP apenas se o processo está rodando */
static void para(int i) {
    if (rodando[i]) {
        kill(pids[i], SIGSTOP);
        rodando[i] = 0;
        printf("[escalonador] P%d STOP (pid=%d)\n", i + 1, pids[i]);
        fflush(stdout);
    }
}

/* Ao receber Ctrl+C, acorda e termina todos os filhos */
static void encerra(int sig) {
    printf("\n[escalonador] encerrando...\n");
    for (int i = 0; i < N; i++) {
        kill(pids[i], SIGCONT);   /* acorda caso esteja parado */
        kill(pids[i], SIGTERM);
    }
    exit(0);
}

/* Código executado pelos processos filhos */
static void filho(int num) {
    raise(SIGSTOP);   /* filho se para; só executa quando o escalonador mandar SIGCONT */
    while (1) {
        printf("  P%d (pid=%d) executando\n", num, getpid());
        fflush(stdout);
        sleep(1);
    }
}

int main() {
    /* Cria os 3 processos filhos, parando cada um imediatamente após o fork */
    for (int i = 0; i < N; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            exit(1);
        }
        if (pids[i] == 0) {
            filho(i + 1);
        }
        kill(pids[i], SIGSTOP);
    }

    signal(SIGINT, encerra);

    printf("Escalonador iniciado | P1=%d  P2=%d  P3=%d\n",
           pids[0], pids[1], pids[2]);
    printf("P1: seg  5-25 | P2: seg 45-60 | P3: seg 0-5 e 25-45\n\n");
    fflush(stdout);

    struct timeval tv;

    /* Loop infinito do escalonador */
    while (1) {
        gettimeofday(&tv, NULL);
        int seg = (int)(tv.tv_sec % 60);

        if (seg >= 5 && seg < 25) {
            /* janela de P1 */
            continua(0);
            para(1);
            para(2);
        } else if (seg >= 45) {
            /* janela de P2 */
            para(0);
            continua(1);
            para(2);
        } else {
            /* P3 preenche o restante: segundos 0-5 e 25-45 */
            para(0);
            para(1);
            continua(2);
        }

        usleep(100000);   /* verifica a cada 100ms */
    }

    return 0;
}
