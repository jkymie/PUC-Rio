#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void) {
    int n = 1;

    pid_t pid_filho = fork();
    if(pid_filho > 0) { // pai porque se fosse filho, seria 0 (o fork retorna dependendo do processo e de onde está chamando)
        for(int i = 0; i < 1000; i++) {
            n++;
        }
        printf("processo pai, pid=%d, n=%d\n", getpid(), n);
        waitpid(pid_filho, NULL, 0);
    }
    else {
        pid_t pid_neto = fork();
        if(pid_neto > 0) {
            for(int i = 0; i < 1000; i++) {
                n = 10 + n;
            }
            printf("processo filho, pid=%d, n=%d\n", getpid(), n);
            waitpid(pid_neto, NULL, 0);
        }
        else {
            for(int i = 0; i < 1000; i++) {
                n = 100 + n;
            }
            printf("processo neto, pid=%d, n=%d\n", getpid(), n);
        }
    }

    return 0;
}