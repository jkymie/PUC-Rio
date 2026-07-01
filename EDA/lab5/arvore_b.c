#include <stdio.h>
#include <stdlib.h>

#define MAX 4
#define MIN 2

typedef struct no t_no;
struct no {
    int ndesc;
    int chave[MAX];
    t_no *ramo[MAX + 1];
};

static t_no *cria_no(int ndesc, int c0, int c1, int c2, int c3, t_no *r0, t_no *r1, t_no *r2, t_no *r3, t_no *r4) {
    t_no *no = (t_no *)malloc(sizeof(t_no));
    no->ndesc   = ndesc;
    no->chave[0] = c0; 
    no->chave[1] = c1;
    no->chave[2] = c2; 
    no->chave[3] = c3;
    no->ramo[0] = r0; 
    no->ramo[1] = r1; 
    no->ramo[2] = r2;
    no->ramo[3] = r3; 
    no->ramo[4] = r4;
    return no;
}

void intervalo(t_no *arv, int lim_inf, int lim_sup) {
    if (arv == NULL) return;

    for (int i = 0; i < arv->ndesc; i++) {
        intervalo(arv->ramo[i], lim_inf, lim_sup);
        if (arv->chave[i] > lim_inf && arv->chave[i] < lim_sup)
            printf("%d\n", arv->chave[i]);
    }
    /* último ramo (à direita da última chave) */
    intervalo(arv->ramo[arv->ndesc], lim_inf, lim_sup);
}

static void libera(t_no *arv) {
    if (arv == NULL) return;
    int i;
    for (i = 0; i <= arv->ndesc; i++)
        libera(arv->ramo[i]);
    free(arv);
}

static t_no *constroi_arvore(void) {
    /* folhas da subárvore esquerda */
    t_no *f1 = cria_no(2, 10,  40,  0,   0,
                       NULL, NULL, NULL, NULL, NULL);
    t_no *f2 = cria_no(2, 60,  70,  0,   0,
                       NULL, NULL, NULL, NULL, NULL);
    t_no *f3 = cria_no(2, 80,  90,  0,   0,
                       NULL, NULL, NULL, NULL, NULL);

    /* folhas da subárvore direita */
    t_no *f4 = cria_no(2, 110, 115, 0,   0,
                       NULL, NULL, NULL, NULL, NULL);
    t_no *f5 = cria_no(4, 130, 135, 140, 170,
                       NULL, NULL, NULL, NULL, NULL);
    t_no *f6 = cria_no(4, 220, 230, 240, 250,
                       NULL, NULL, NULL, NULL, NULL);

    /* nós internos de nível 2 */
    t_no *n1 = cria_no(2, 50, 75, 0, 0,
                       f1, f2, f3, NULL, NULL);
    t_no *n2 = cria_no(2, 120, 200, 0, 0,
                       f4, f5, f6, NULL, NULL);

    /* raiz */
    t_no *raiz = cria_no(1, 100, 0, 0, 0,
                         n1, n2, NULL, NULL, NULL);
    return raiz;
}

int main(void) {
    t_no *raiz = constroi_arvore();

    printf("Intervalo (5, 300):\n");
    intervalo(raiz, 5, 300);

    printf("\nIntervalo (50, 100):\n");
    intervalo(raiz, 50, 100);

    printf("\nIntervalo (70, 135):\n");
    intervalo(raiz, 70, 135);

    libera(raiz);
    return 0;
}