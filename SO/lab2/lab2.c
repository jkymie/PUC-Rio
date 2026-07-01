#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(void) {
    int n = 1;
    pid_t pid1 = fork();
    if(pid1 == 0) {
        for(int i = 0; i < 1000; i++) {
            n++;
            printf("processo filho1, pid=%d, n=%d", getpid(), n);
        }
        exit(0);
    }
    pid_t pid2 = fork();
    if(pid2 == 0) {
        for(int i = 0; i < 1000; i++) {
            n = 10 + n;
            printf("processo filho2, pid=%d, n=%d", getpid(), n);
        }
        exit(0);
    }
    pid_t pid3 = fork();
    if(pid3 == 0) {
        for(int i = 0; i < 1000; i++) {
            n = 100 + n;
            printf("processo filho3, pid=%d, n=%d", getpid(), n);
        }
        exit(0);
    }
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);
    return 0;
}