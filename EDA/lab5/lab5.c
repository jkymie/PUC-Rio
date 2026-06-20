#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct bitvector {
    int n;
    unsigned char *v;
};
typedef struct bitvector BitVector;

BitVector *set_new(int nbytes, unsigned char *data) {
    BitVector *s = malloc(sizeof(BitVector));
    s->n = nbytes;
    s->v = malloc(nbytes);
    if (data)
        memcpy(s->v, data, nbytes);
    else
        memset(s->v, 0, nbytes);
    return s;
}

void set_free(BitVector *s) {
    free(s->v);
    free(s);
}

void set_print(const BitVector *s) {
    for (int i = 0; i < s->n; i++) {
        printf("%02X", s->v[i]);
        if ((i % 2 == 1) && (i < s->n - 1))
            printf(" ");
    }
    printf("\n");
}

static BitVector *set_copy(const BitVector *s) {
    return set_new(s->n, s->v);
}

/* remove o i-ésimo elemento do conjunto s */
BitVector *set_remove(BitVector *s, int i) {
    s->v[i / 8] &= ~(1 << (i % 8));
    return s;
}

/* inclui o i-ésimo elemento no conjunto s */
BitVector *set_inclui(BitVector *s, int i) {
    s->v[i / 8] |= (1 << (i % 8));
    return s;
}

/* retorna t = elementos de r que não estão em s */
BitVector *set_diferenca(BitVector *r, BitVector *s) {
    BitVector *t = set_copy(r);
    int min = t->n < s->n ? t->n : s->n;
    for (int i = 0; i < min; i++)
        t->v[i] &= ~s->v[i];
    return t;
}

/* retorna t = r união s */
BitVector *set_uniao(BitVector *r, BitVector *s) {
    int max = r->n > s->n ? r->n : s->n;
    BitVector *t = set_new(max, NULL);
    for (int i = 0; i < r->n; i++)
        t->v[i] |= r->v[i];
    for (int i = 0; i < s->n; i++)
        t->v[i] |= s->v[i];
    return t;
}

int main(void) {
    unsigned char sv[] = {0x00, 0x00, 0x00, 0xFA, 0x11, 0xFF, 0xAA, 0xBB, 0xCC, 0xDD};
    unsigned char rv[] = {0x05, 0xEE, 0x00, 0xF1, 0x00, 0x11, 0x44, 0x55, 0x00, 0x00};

    BitVector *s = set_new(10, sv);
    BitVector *r = set_new(10, rv);

    printf("s = ");
    set_print(s);
    printf("r = ");
    set_print(r);

    /* --- a) set_remove --- */
    printf("\n--- a) set_remove: removendo posicoes 30, 39 e 73 de s ---\n");
    set_remove(s, 30);
    set_remove(s, 39);
    set_remove(s, 73);
    printf("s = ");
    set_print(s);

    /* --- b) set_inclui --- */
    printf("\n--- b) set_inclui: incluindo posicoes 30, 39 e 73 em s ---\n");
    set_inclui(s, 30);
    set_inclui(s, 39);
    set_inclui(s, 73);
    printf("s = ");
    set_print(s);

    /* --- c) set_diferenca --- */
    printf("\n--- c) set_diferenca: s - r ---\n");
    BitVector *dif = set_diferenca(s, r);
    printf("s     = ");
    set_print(s);
    printf("r     = ");
    set_print(r);
    printf("s - r = ");
    set_print(dif);
    set_free(dif);

    /* --- d) set_uniao --- */
    printf("\n--- d) set_uniao: r u s ---\n");
    BitVector *uni = set_uniao(r, s);
    printf("r u s = ");
    set_print(uni);
    set_free(uni);

    set_free(s);
    set_free(r);
    return 0;
}
