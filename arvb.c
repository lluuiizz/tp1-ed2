#include "include/arvb.h"
#include <stdlib.h>

// Contadores para pré-processamento (construção da árvore)
int TRANSF_EXT_INT_ARVB_PRE = 0; // Transferências de páginas durante a construção
int COMP_CHAVES_ARVB_PRE = 0;    // Comparações de chaves durante a construção

// Contadores para pós-processamento (busca)
int TRANSF_EXT_INT_ARVB_POS = 0; // Transferências de páginas durante a busca
int COMP_CHAVES_ARVB_POS = 0;    // Comparações de chaves durante a busca

// Inicializa a árvore B (ponteiro para NULL)
void inicializa(tipo_apontador *arvore) {
    *arvore = NULL;
}

// Busca uma chave na árvore B (pós-processamento)
// Retorna true se encontrou, false caso contrário
bool pesquisa_arvore_b(tipo_registro *x, tipo_apontador ap) {
    if (ap == NULL) return false;      // Árvore vazia
    if (ap->n == 0) return false;      // Página vazia

    int i = 1;
    // Procura a posição onde a chave pode estar ou deve ser buscada
    while (i < ap->n && (COMP_CHAVES_ARVB_POS++, x->chave > ap->r[i-1].chave)) i++;
    COMP_CHAVES_ARVB_POS++; // Conta a última comparação (==)

    // Se encontrou a chave na página atual
    if (i <= ap->n && x->chave == ap->r[i-1].chave) {
        *x = ap->r[i-1]; // Copia o registro encontrado
        return true;
    }

    // Conta transferência de página (descendo para um filho)
    TRANSF_EXT_INT_ARVB_POS++;

    // Decide para qual filho descer
    if (i == 1)
        return pesquisa_arvore_b(x, ap->p[0]);      // Filho mais à esquerda
    else if (i <= ap->n)
        return pesquisa_arvore_b(x, ap->p[i]);      // Filho intermediário
    else
        return pesquisa_arvore_b(x, ap->p[ap->n]);  // Filho mais à direita
}

// exibir_reg_arvb as chaves da árvore B em ordem (percurso em ordem)
void exibir_reg_arvb(tipo_apontador arvore) {
    if (arvore == NULL) return;
    int i = 0;
    while (i <= arvore->n) {
        exibir_reg_arvb(arvore->p[i]); // imprime a subárvore à esquerda
        if (i != arvore->n)
            printf("%d ", arvore->r[i].chave); // imprime a chave do nó
        i++;
    }
}

// Insere um registro em uma página (nó) da árvore B
void insere_na_pagina(tipo_apontador ap, tipo_registro reg, tipo_apontador ap_dir) {
    int k = ap->n;
    // Move os registros maiores para a direita para abrir espaço
    while (k > 0 && (COMP_CHAVES_ARVB_PRE++, reg.chave < ap->r[k-1].chave)) {
        ap->r[k] = ap->r[k-1];
        ap->p[k+1] = ap->p[k];
        k--;
    }
    // Insere o novo registro e o ponteiro à direita
    ap->r[k] = reg;
    ap->p[k+1] = ap_dir;
    ap->n++;
    // Conta transferência de página (inserção)
    TRANSF_EXT_INT_ARVB_PRE++;
}

// Função recursiva de inserção na árvore B
// Pode causar split e propagação para cima
void ins(tipo_registro reg, tipo_apontador ap, short *cresceu, tipo_registro *reg_retorno, tipo_apontador *ap_retorno) {
    long i = 1, j;
    tipo_apontador ap_temp;
    if (ap == NULL) {
        // Caso base: chegou em um ponteiro nulo, precisa criar novo nó
        *cresceu = 1;
        *reg_retorno = reg;
        *ap_retorno = NULL;
        return;
    }
    // Busca a posição correta para inserir
    while (i < ap->n && (COMP_CHAVES_ARVB_PRE++, reg.chave > ap->r[i-1].chave)) i++;
    COMP_CHAVES_ARVB_PRE++; // Conta a última comparação (==)
    if (reg.chave == ap->r[i-1].chave) {
        // Chave já existe, não insere duplicado
        *cresceu = 0;
        return;
    }
    if (reg.chave < ap->r[i-1].chave) i--;
    // Conta transferência de página (descendo para filho)
    TRANSF_EXT_INT_ARVB_PRE++;
    // Chama recursivamente para o filho apropriado
    ins(reg, ap->p[i], cresceu, reg_retorno, ap_retorno);
    if (!*cresceu) return;

    // Se não precisa split, apenas insere na página
    if (ap->n < ORDEM) {
        insere_na_pagina(ap, *reg_retorno, *ap_retorno);
        *cresceu = 0;
        return;
    }

    // Split: página cheia, precisa dividir
    ap_temp = (tipo_apontador) malloc(sizeof(struct tipo_pagina));
    ap_temp->n = 0;
    // Inicializa todos os ponteiros filhos com NULL
    for (int idx = 0; idx < ORDEM + 1; idx++)
        ap_temp->p[idx] = NULL;

    int meio = ORDEM / 2;
    if (i <= meio) {
        // Novo registro vai para a página original
        insere_na_pagina(ap_temp, ap->r[ORDEM-1], ap->p[ORDEM]);
        ap->n--;
        insere_na_pagina(ap, *reg_retorno, *ap_retorno);
    } else {
        // Novo registro vai para a nova página
        insere_na_pagina(ap_temp, *reg_retorno, *ap_retorno);
    }
    // Move metade superior dos registros para a nova página
    for (j = ORDEM-1; j >= meio+1; j--)
        insere_na_pagina(ap_temp, ap->r[j-1], ap->p[j]);
    ap->n = meio;
    ap_temp->p[0] = ap->p[meio+1];
    *reg_retorno = ap->r[meio]; // Registro do meio sobe
    *ap_retorno = ap_temp;      // Nova página à direita
}

// Insere um registro na árvore B (pode criar nova raiz)
void insere(tipo_registro reg, tipo_apontador *ap) {
    short cresceu;
    tipo_registro reg_retorno;
    tipo_apontador ap_retorno, ap_temp;
    ins(reg, *ap, &cresceu, &reg_retorno, &ap_retorno);
    if (cresceu) {
        // Se cresceu na raiz, cria nova raiz
        ap_temp = (tipo_apontador) malloc(sizeof(struct tipo_pagina));
        ap_temp->n = 1;
        ap_temp->r[0] = reg_retorno;
        for (int idx = 0; idx < ORDEM + 1; idx++)
            ap_temp->p[idx] = NULL;
        ap_temp->p[1] = ap_retorno;
        ap_temp->p[0] = *ap;
        *ap = ap_temp;
        // Conta transferência de página (nova raiz)
        TRANSF_EXT_INT_ARVB_PRE++;
    }
}

// Libera toda a memória da árvore B (recursivo)
void libera_arvore_b(tipo_apontador arvore) {
    if (arvore == NULL) return;
    for (int i = 0; i <= arvore->n; i++) {
        if (arvore->p[i] != NULL) {
            libera_arvore_b(arvore->p[i]);
            arvore->p[i] = NULL;
        }
    }
    free(arvore);
}

// Constrói a árvore B a partir de um arquivo binário
// Lê 'quantidade' registros do arquivo e insere na árvore
tipo_apontador construir_arvore_b(const char *nomeArquivo, int quantidade) {
    FILE *arq = fopen(nomeArquivo, "rb");
    if (arq == NULL) {
        printf("Erro ao abrir arquivo %s\n", nomeArquivo);
        return NULL;
    }

    tipo_apontador arvore;
    inicializa(&arvore);

    reg_t temp;
    for (int i = 0; i < quantidade; i++) {
        fread(&temp, sizeof(reg_t), 1, arq);
        tipo_registro reg;
        reg.chave = temp.chave;
        insere(reg, &arvore);
    }
    fclose(arq);
    return arvore;
}

// Exibe os contadores de operações da árvore B
void print_counters_arvb() {
    printf("Pré-processamento:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_ARVB_PRE);
    printf("  Comparações: %d\n", COMP_CHAVES_ARVB_PRE);
    printf("Pós-processamento:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_ARVB_POS);
    printf("  Comparações: %d\n", COMP_CHAVES_ARVB_POS);
    printf("Total:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_ARVB_PRE + TRANSF_EXT_INT_ARVB_POS);
    printf("  Comparações: %d\n", COMP_CHAVES_ARVB_PRE + COMP_CHAVES_ARVB_POS);
}