#include <stdio.h>
#include <stdlib.h>

struct set {
    int n; /* número de bytes */
    unsigned char* v; /* vetor */
};
typedef struct set Set;

int contem(Set* A, Set* B) {
    int min;
    if (A->n < B->n) {
        min = A->n;
    } else {
        min = B->n;
    }

    for (int i = 0; i < min; i++) {
        if ((A->v[i] & B->v[i]) != B->v[i]) {
            return 0;
        }
    }

    for (int i = min; i < B->n; i++) {
        if (B->v[i] != 0) {
            return 0;
        }
    }

    return 1;
}

int main() {
    /* A = {0, 1, 3}, B = {0, 3}, C = {2} */
    Set A, B, C;
    unsigned char va[] = {0b00001011};
    unsigned char vb[] = {0b00001001};
    unsigned char vc[] = {0b00000100};

    A.n = 1; A.v = va;
    B.n = 1; B.v = vb;
    C.n = 1; C.v = vc;

    printf("A contem B? %d\n", contem(&A, &B)); /* resultado: 1 */
    printf("A contem C? %d\n", contem(&A, &C)); /* resultado: 0 */
    printf("B contem A? %d\n", contem(&B, &A)); /* resultado: 0 */

    return 0;
}
