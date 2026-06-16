#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// cada no da arvore = o caractere, sua frequencia e os filhos
typedef struct Node {
    unsigned char ch;
    int freq;
    struct Node *esq, *dir;
} Node;

// caractere = seu codigo binario de Huffman
typedef struct {
    unsigned char ch;
    char codigo[256];
    int bits;
} Codigo;

Codigo tabelaCodigos[256];
int totalCodigos = 0;

Node *criarNo(unsigned char ch, int freq) {
    Node *no = malloc(sizeof(Node));
    no->ch   = ch;
    no->freq = freq;
    no->esq  = NULL;
    no->dir  = NULL;
    return no;
}

void libera(Node *raiz) {
    if (raiz == NULL) return;
    libera(raiz->esq);
    libera(raiz->dir);
    free(raiz);
}

// acha o indice do no com menor frequencia
int encontrarMinimo(Node *nos[], int n) {
    int minIdx = 0;
    for (int i = 1; i < n; i++)
        if (nos[i]->freq < nos[minIdx]->freq)
            minIdx = i;
    return minIdx;
}

// constroi a arvore de Huffman juntando os dois nos de menor frequencia
Node *construirArvore(int freq[]) {
    Node *nos[512];
    int n = 0;

    // cria um no folha para cada caractere que aparece no texto
    for (int i = 0; i < 256; i++)
        if (freq[i] > 0)
            nos[n++] = criarNo((unsigned char)i, freq[i]);

    // fica repetindo ate sobrar so a raiz
    while (n > 1) {
        int idxA = encontrarMinimo(nos, n);
        Node *a  = nos[idxA];
        nos[idxA] = nos[--n];

        int idxB = encontrarMinimo(nos, n);
        Node *b  = nos[idxB];
        nos[idxB] = nos[--n];

        // novo no pai com frequencia igual a soma dos dois filhos
        Node *pai = criarNo(0, a->freq + b->freq);
        pai->esq  = a;
        pai->dir  = b;
        nos[n++]  = pai;
    }

    return nos[0];
}

// percorre a arvore recursivamente: esquerda = 0, direita = 1
void gerarCodigos(Node *raiz, char *buf, int profundidade) {
    if (raiz->esq == NULL && raiz->dir == NULL) {
        // chegou numa folha: salva o codigo gerado ate aqui
        buf[profundidade] = '\0';
        tabelaCodigos[totalCodigos].ch = raiz->ch;
        strcpy(tabelaCodigos[totalCodigos].codigo, buf);
        tabelaCodigos[totalCodigos].bits = profundidade;
        totalCodigos++;
        return;
    }

    buf[profundidade] = '0';
    gerarCodigos(raiz->esq, buf, profundidade + 1);

    buf[profundidade] = '1';
    gerarCodigos(raiz->dir, buf, profundidade + 1);
}

const char *buscarCodigo(unsigned char c) {
    for (int i = 0; i < totalCodigos; i++)
        if (tabelaCodigos[i].ch == c)
            return tabelaCodigos[i].codigo;
    return NULL;
}

void imprimirCaractere(unsigned char c) {
    if (c == '\n') {
        printf("'\\n'");
    } else if (c == '\r') {
        printf("'\\r'");
    } else if (c == ' ') {
        printf("' '");
    } else {
        printf("'%c'", c);
    }
}

void imprimirTabelaFreq(int freq[], int total) {
    printf("\n=== TABELA DE FREQUENCIAS ===\n");
    printf("%-12s %10s\n", "Caractere", "Frequencia");
    printf("%-12s %10s\n", "----------", "----------");
    for (int i = 0; i < 256; i++) {
        if (freq[i] == 0) continue;
        printf("   ");
        imprimirCaractere((unsigned char)i);
        printf("%-7s %10d\n", "", freq[i]);
    }
    printf("%-12s %10d\n", "   TOTAL", total);
}

void imprimirTabelaCodigos(int freq[]) {
    printf("\n=== TABELA DE CODIGOS HUFFMAN ===\n");
    printf("%-12s %-25s %5s %12s\n", "Caractere", "Codigo Binario", "Bits", "Freq");
    printf("%-12s %-25s %5s %12s\n", "----------", "---------------", "-----", "----------");
    for (int i = 0; i < totalCodigos; i++) {
        unsigned char c = tabelaCodigos[i].ch;
        printf("   ");
        imprimirCaractere(c);
        printf("%-7s %-35s %5d %12d\n", "", tabelaCodigos[i].codigo, tabelaCodigos[i].bits, freq[c]);
    }
}

void compactar(const char *entrada, const char *saida, int freq[]) {
    FILE *arqEntrada = fopen(entrada, "rb");
    FILE *arqSaida   = fopen(saida,   "wb");
    if (!arqEntrada || !arqSaida) {
        printf("Erro ao abrir arquivo.\n");
        return;
    }

    // salva a tabela de frequencias no cabecalho para poder reconstruir a arvore ao descompactar
    int unicos = 0;
    for (int i = 0; i < 256; i++) if (freq[i] > 0) unicos++;
    fwrite(&unicos, sizeof(int), 1, arqSaida);

    for (int i = 0; i < 256; i++) {
        if (freq[i] == 0) continue;
        unsigned char c = (unsigned char)i;
        fwrite(&c,       sizeof(unsigned char), 1, arqSaida);
        fwrite(&freq[i], sizeof(int),           1, arqSaida);
    }

    // conta quantos bits o arquivo comprimido vai ter e salva no cabecalho
    int totalBits = 0;
    int ch;
    while ((ch = fgetc(arqEntrada)) != EOF) {
        const char *cod = buscarCodigo((unsigned char)ch);
        if (cod) totalBits += strlen(cod);
    }
    fwrite(&totalBits, sizeof(int), 1, arqSaida);

    // le o arquivo de novo e escreve os bits empacotados em bytes
    rewind(arqEntrada);
    unsigned char byte   = 0;
    int           bitPos = 0;

    while ((ch = fgetc(arqEntrada)) != EOF) {
        const char *cod = buscarCodigo((unsigned char)ch);
        if (!cod) continue;

        for (int i = 0; cod[i] != '\0'; i++) {
            if (cod[i] == '1')
                byte |= (unsigned char)(1 << (7 - bitPos));

            bitPos++;
            if (bitPos == 8) {
                fwrite(&byte, 1, 1, arqSaida);
                byte   = 0;
                bitPos = 0;
            }
        }
    }
    // escreve o ultimo byte mesmo que incompleto
    if (bitPos > 0) fwrite(&byte, 1, 1, arqSaida);

    fclose(arqEntrada);
    fclose(arqSaida);

    FILE *orig = fopen(entrada, "rb");
    FILE *comp = fopen(saida,   "rb");
    fseek(orig, 0, SEEK_END); long tamanhoOrig = ftell(orig);
    fseek(comp, 0, SEEK_END); long tamanhoComp = ftell(comp);
    fclose(orig);
    fclose(comp);

    printf("\n=== ESTATISTICAS DE COMPRESSAO ===\n");
    printf("Tamanho original: %ld bytes  (%ld bits)\n", tamanhoOrig, tamanhoOrig * 8);
    printf("Bits de dados: %d bits (%d bytes arredondados)\n", totalBits, (totalBits + 7) / 8);
    printf("Arquivo comprimido (.huf): %ld bytes  (dados + cabecalho)\n", tamanhoComp);
    printf("Arquivo comprimido salvo: %s\n", saida);
}

void descompactar(const char *entrada, const char *saida) {
    FILE *arqEntrada = fopen(entrada, "rb");
    if (!arqEntrada) {
        printf("Erro ao abrir arquivo comprimido.\n");
        return;
    }

    // le a tabela de frequencias do cabecalho para reconstruir a arvore
    int unicos = 0;
    if (fread(&unicos, sizeof(int), 1, arqEntrada) == 0) {
        fclose(arqEntrada); return;
    }

    int freq[256] = {0};
    for (int i = 0; i < unicos; i++) {
        unsigned char c = 0;
        int f = 0;
        if (fread(&c, sizeof(unsigned char), 1, arqEntrada) == 0) break;
        if (fread(&f, sizeof(int),           1, arqEntrada) == 0) break;
        freq[c] = f;
    }

    int totalBits = 0;
    if (fread(&totalBits, sizeof(int), 1, arqEntrada) == 0) {
        fclose(arqEntrada); return;
    }

    Node *raiz = construirArvore(freq);

    FILE *arqSaida = fopen(saida, "wb");
    if (!arqSaida) {
        printf("Erro ao criar arquivo de saida.\n");
        fclose(arqEntrada);
        libera(raiz);
        return;
    }

    // percorre os bits: 0 vai para esquerda, 1 vai para direita
    // quando chega numa folha, o caractere foi decodificado
    Node *atual    = raiz;
    int   bitsLidos = 0;
    unsigned char byte;

    while (bitsLidos < totalBits && fread(&byte, 1, 1, arqEntrada) == 1) {
        for (int bit = 7; bit >= 0 && bitsLidos < totalBits; bit--) {
            int valorBit = (byte >> bit) & 1;

            if (valorBit == 0) atual = atual->esq;
            else               atual = atual->dir;

            bitsLidos++;

            if (atual->esq == NULL && atual->dir == NULL) {
                fputc(atual->ch, arqSaida);
                atual = raiz; // volta para a raiz para decodificar o proximo caractere
            }
        }
    }

    fclose(arqEntrada);
    fclose(arqSaida);
    libera(raiz);
    printf("Arquivo descomprimido salvo: %s\n", saida);
}

void verificar(const char *original, const char *descomprimido) {
    FILE *a = fopen(original,      "rb");
    FILE *b = fopen(descomprimido, "rb");
    if (!a || !b) {
        printf("Erro ao abrir arquivos para verificacao.\n");
        return;
    }

    int igual = 1;
    int ca, cb;
    long posicao = 0;

    while ((ca = fgetc(a)) != EOF && (cb = fgetc(b)) != EOF) {
        if (ca != cb) {
            printf("Diferenca encontrada na posicao %ld.\n", posicao);
            igual = 0;
            break;
        }
        posicao++;
    }

    if (igual && (fgetc(a) != EOF || fgetc(b) != EOF))
        igual = 0;

    fclose(a);
    fclose(b);

    printf("\n=== VERIFICACAO\n");
    if (igual)
        printf("Deu certo!\n");
    else
        printf("Falhou!\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        printf("Erro ao abrir '%s'.\n", argv[1]);
        return 1;
    }

    // conta quantas vezes cada caractere aparece no texto
    int freq[256] = {0};
    int total = 0;
    int ch;
    while ((ch = fgetc(f)) != EOF) {
        freq[(unsigned char)ch]++;
        total++;
    }
    fclose(f);

    printf("Total de caracteres: %d\n", total);

    imprimirTabelaFreq(freq, total);

    // constroi a arvore e gera os codigos binarios para cada caractere
    Node *raiz = construirArvore(freq);
    char buf[256];
    gerarCodigos(raiz, buf, 0);

    imprimirTabelaCodigos(freq);

    compactar(argv[1], "textoComprimido.huf", freq);
    descompactar("textoComprimido.huf", "textoDescomprimido.txt");
    verificar(argv[1], "textoDescomprimido.txt");

    libera(raiz);
    return 0;
}