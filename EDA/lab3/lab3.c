#include <stdio.h>
#include <stdlib.h>

struct nodo {
    int chave;
    int altura;
    struct nodo *esq;
    struct nodo *dir;
};

typedef struct nodo Nodo;

Nodo *criar_no(int chave) {
    Nodo *novo = (Nodo *)malloc(sizeof(Nodo));
    novo->chave = chave;
    novo->altura = 0;
    novo->esq = NULL;
    novo->dir = NULL;
    return novo;
}

Nodo *inserir(Nodo *raiz, int chave) {
    if (raiz == NULL)
        return criar_no(chave);
    if (chave < raiz->chave)
        raiz->esq = inserir(raiz->esq, chave);
    else if (chave > raiz->chave)
        raiz->dir = inserir(raiz->dir, chave);
    return raiz;
}

int calcular_altura(Nodo *raiz) {
    if (raiz == NULL)
        return -1;
    int h_esq = calcular_altura(raiz->esq);
    int h_dir = calcular_altura(raiz->dir);
    raiz->altura = (h_esq > h_dir ? h_esq : h_dir) + 1;
    return raiz->altura;
}

/* Pre-ordem: raiz, esquerda, direita */
void pre_ordem(Nodo *raiz) {
    if (raiz == NULL) return;
    printf("%d(%d) ", raiz->chave, raiz->altura);
    pre_ordem(raiz->esq);
    pre_ordem(raiz->dir);
}

/* Ordem simetrica: esquerda, raiz, direita */
void ordem_simetrica(Nodo *raiz) {
    if (raiz == NULL) return;
    ordem_simetrica(raiz->esq);
    printf("%d(%d) ", raiz->chave, raiz->altura);
    ordem_simetrica(raiz->dir);
}

typedef struct fila_nodo {
    Nodo *nodo;
    struct fila_nodo *prox;
} FilaNodo;

void enfileirar(FilaNodo **frente, FilaNodo **fundo, Nodo *nodo) {
    FilaNodo *novo = (FilaNodo *)malloc(sizeof(FilaNodo));
    novo->nodo = nodo;
    novo->prox = NULL;
    if (*fundo == NULL)
        *frente = *fundo = novo;
    else {
        (*fundo)->prox = novo;
        *fundo = novo;
    }
}

Nodo *desenfileirar(FilaNodo **frente, FilaNodo **fundo) {
    if (*frente == NULL) return NULL;
    FilaNodo *temp = *frente;
    Nodo *nodo = temp->nodo;
    *frente = (*frente)->prox;
    if (*frente == NULL) *fundo = NULL;
    free(temp);
    return nodo;
}

void por_nivel(Nodo *raiz) {
    if (raiz == NULL) return;
    FilaNodo *frente = NULL, *fundo = NULL;
    enfileirar(&frente, &fundo, raiz);
    while (frente != NULL) {
        Nodo *atual = desenfileirar(&frente, &fundo);
        printf("%d(%d) ", atual->chave, atual->altura);
        if (atual->esq != NULL) enfileirar(&frente, &fundo, atual->esq);
        if (atual->dir != NULL) enfileirar(&frente, &fundo, atual->dir);
    }
}

void liberar(Nodo *raiz) {
    if (raiz == NULL) return;
    liberar(raiz->esq);
    liberar(raiz->dir);
    free(raiz);
}

int main() {
    FILE *arquivo = fopen("chaves.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo chaves.txt\n");
        return 1;
    }

    Nodo *raiz = NULL;
    int number;

    while (fscanf(arquivo, "%d", &number) == 1)
        raiz = inserir(raiz, number);

    fclose(arquivo);

    calcular_altura(raiz);

    printf("Pre-ordem: ");
    pre_ordem(raiz);
    printf("\n");

    printf("Ordem Simetrica: ");
    ordem_simetrica(raiz);
    printf("\n");

    printf("Por nivel: ");
    por_nivel(raiz);
    printf("\n");

    liberar(raiz);
    return 0;
}