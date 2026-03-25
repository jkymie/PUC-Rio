#include <stdio.h>
#define N 50
#include <stdlib.h>

struct elemento {
    char info;
    struct elemento *prox;
};
typedef struct elemento Elemento;

struct pilha {
    Elemento* topo;
};
typedef struct pilha Pilha;

Pilha* p = NULL; // pilha é global para funcionar 

/* inicializar pilha com 0 elementos */
void cria(void)
{
    p = (Pilha*) malloc(sizeof(Pilha));
    if (p==NULL) {
        exit(1);
    }
    p->topo = NULL;
}

/* função de empilhar */
void push(char c) {
    Elemento* n = (Elemento*) malloc(sizeof(Elemento));
    if (n==NULL){ 
        printf("Sem memória para alocar elemento.\n");
        exit(1);
    }
    n->info = c; // info do topo/novo elemento é o valor c
    n->prox = p->topo; // o próximo é quem está embaixo do topo
    p->topo = n; // novo topo é o elemento atual
}

/* função de desempilhar */
char pop() {
    if (p->topo == NULL) {
        printf("Pilha vazia!\n");
        return '\0';
    }
    Elemento* t = p->topo; // guardamos o endereço do elemento do topo da pilha
    char c = t->info; // pegamos o valor da info do topo para exibir
    p->topo = t->prox; // 
    free(t);
    return c;
}

void exibe() {
    Elemento* atual = p->topo;
    char valores[N];
    int i = 0;
    while (atual != NULL && i < N) {
        valores[i++] = atual->info;
        atual = atual->prox;
    }
    printf("[");
    for (int j = i-1; j >= 0; j--) {
        printf("%c", valores[j]);
        if (j > 0) printf(", ");
    }
    printf("]\n");
}

int main() {
    FILE* f = fopen("lista.txt", "r");
    if (!f) {
        printf("Erro ao abrir lista.txt\n");
        return 1;
    }
    cria();
    char op[8];
    while (fscanf(f, "%7s", op) == 1) {
        if (op[0] == 'e') {
            // deve empilhar
            push(op[1]);
            exibe();
        } else if (op[0] == 'r') {
            // deve desempilhar
            char removido = pop();
            exibe();
        }
    }
    fclose(f);
    // Libera memória
    while (p->topo != NULL) {
        pop();
    }
    free(p);
    return 0;
}
