#include "include/arvb-star.h"
#include <stdlib.h>

// Contadores para pré-processamento (construção da árvore B*)
int TRANSF_EXT_INT_BSTAR_PRE = 0;
int COMP_CHAVES_BSTAR_PRE = 0;

// Contadores para pós-processamento (busca)
int TRANSF_EXT_INT_BSTAR_POS = 0;
int COMP_CHAVES_BSTAR_POS = 0;

// Inicializa a árvore B* (define ponteiro raiz como NULL)
void inicializa_b_estrela(tipo_apontador_estrela *arvore) {
    *arvore = NULL;
}

// Busca uma chave na árvore B* (igual à B)
bool pesquisa_b_estrela(tipo_registro_estrela *x, tipo_apontador_estrela ap) {
    if (ap == NULL) return false;
    int i = 0;
    while (i < ap->n && (COMP_CHAVES_BSTAR_POS++, x->chave > ap->r[i].chave)) i++;
    COMP_CHAVES_BSTAR_POS++; // Conta a última comparação (==)
    if (i < ap->n && x->chave == ap->r[i].chave) {
        *x = ap->r[i];
        return true;
    }
    TRANSF_EXT_INT_BSTAR_POS++; // Transferência ao descer para filho
    return pesquisa_b_estrela(x, ap->p[i]);
}

// Imprime as chaves da árvore B* em ordem
void exibir_reg_arvbstar(tipo_apontador_estrela arvore) {
    if (!arvore) return;
    int i;
    for (i = 0; i < arvore->n; i++) {
        exibir_reg_arvbstar(arvore->p[i]);
        printf("%d ", arvore->r[i].chave);
    }
    exibir_reg_arvbstar(arvore->p[i]);
}

// Insere um registro em uma página
void insere_na_pagina_estrela(tipo_apontador_estrela ap, tipo_registro_estrela reg, tipo_apontador_estrela ap_dir) {
    int k = ap->n;
    while (k > 0 && (COMP_CHAVES_BSTAR_PRE++, reg.chave < ap->r[k-1].chave)) {
        ap->r[k] = ap->r[k-1];
        ap->p[k+1] = ap->p[k];
        k--;
    }
    ap->r[k] = reg;
    ap->p[k+1] = ap_dir;
    ap->n++;
    TRANSF_EXT_INT_BSTAR_PRE++; // Transferência ao inserir em página
}

// Redistribui entre irmãos (esquerda ou direita)
static int redistribui(tipo_apontador_estrela pai, int pos, tipo_apontador_estrela filho, tipo_apontador_estrela irmao, int direita) {
    if (!irmao) return 0;
    if (irmao->n < ORDEM_BSTAR - 1) {
        if (direita) {
            for (int j = irmao->n; j > 0; j--) {
                irmao->r[j] = irmao->r[j-1];
                irmao->p[j+1] = irmao->p[j];
            }
            irmao->p[1] = irmao->p[0];
            irmao->r[0] = pai->r[pos];
            irmao->p[0] = filho->p[filho->n];
            irmao->n++;
            pai->r[pos] = filho->r[filho->n-1];
            filho->n--;
            TRANSF_EXT_INT_BSTAR_PRE++; // Transferência ao redistribuir
            return 1;
        } else {
            irmao->r[irmao->n] = pai->r[pos-1];
            irmao->p[irmao->n+1] = filho->p[0];
            irmao->n++;
            pai->r[pos-1] = filho->r[0];
            for (int j = 0; j < filho->n-1; j++) {
                filho->r[j] = filho->r[j+1];
                filho->p[j] = filho->p[j+1];
            }
            filho->p[filho->n-1] = filho->p[filho->n];
            filho->n--;
            TRANSF_EXT_INT_BSTAR_PRE++; // Transferência ao redistribuir
            return 1;
        }
    }
    return 0;
}

// Split real da B* (dinâmico para qualquer ordem)
static void split_bstar(tipo_apontador_estrela pai, int pos, tipo_apontador_estrela filho, tipo_registro_estrela reg, tipo_apontador_estrela ap_dir, tipo_registro_estrela *reg_sobe, tipo_apontador_estrela *novo_dir) {
    int t = ORDEM_BSTAR;
    tipo_registro_estrela temp[t+1];
    tipo_apontador_estrela temp_p[t+2];
    int i, j;

    for (i = 0; i < filho->n; i++) temp[i] = filho->r[i];
    temp[i] = reg;
    for (i = 0; i <= filho->n; i++) temp_p[i] = filho->p[i];
    temp_p[filho->n+1] = ap_dir;

    // Ordena as chaves temporárias (inserção já ordenada, mas por segurança)
    for (i = 0; i < filho->n; i++) {
        for (j = i+1; j < filho->n+1; j++) {
            if (temp[i].chave > temp[j].chave) {
                tipo_registro_estrela aux = temp[i];
                temp[i] = temp[j];
                temp[j] = aux;
            }
        }
    }

    int meio = t / 2;
    tipo_apontador_estrela nova = (tipo_apontador_estrela)malloc(sizeof(tipo_pagina_estrela));
    nova->n = 0;
    for (i = 0; i < ORDEM_BSTAR+1; i++) nova->p[i] = NULL;

    filho->n = meio;
    for (i = 0; i < meio; i++) {
        filho->r[i] = temp[i];
        filho->p[i] = temp_p[i];
    }
    filho->p[meio] = temp_p[meio];

    nova->n = t - meio;
    for (i = 0; i < nova->n; i++) {
        nova->r[i] = temp[meio + 1 + i];
        nova->p[i] = temp_p[meio + 1 + i];
    }
    nova->p[nova->n] = temp_p[t + 1];

    *reg_sobe = temp[meio];
    *novo_dir = nova;
    TRANSF_EXT_INT_BSTAR_PRE++; // Transferência ao split
}

// Inserção recursiva com redistribuição real de B*
static void insere_bstar_rec(tipo_apontador_estrela pai, int pos_pai, tipo_registro_estrela reg, tipo_apontador_estrela ap, int *cresceu, tipo_registro_estrela *reg_sobe, tipo_apontador_estrela *novo_dir) {
    if (ap == NULL) {
        *cresceu = 1;
        *reg_sobe = reg;
        *novo_dir = NULL;
        return;
    }
    int i = 0;
    while (i < ap->n && (COMP_CHAVES_BSTAR_PRE++, reg.chave > ap->r[i].chave)) i++;
    COMP_CHAVES_BSTAR_PRE++; // Conta a última comparação (==)
    if (i < ap->n && reg.chave == ap->r[i].chave) {
        *cresceu = 0;
        return;
    }
    TRANSF_EXT_INT_BSTAR_PRE++; // Transferência ao descer para filho
    insere_bstar_rec(ap, -1, reg, ap->p[i], cresceu, reg_sobe, novo_dir);
    if (!*cresceu) return;

    if (pai && pos_pai > 0 && redistribui(pai, pos_pai, ap, pai->p[pos_pai-1], 0)) {
        *cresceu = 0;
        return;
    }
    if (pai && pos_pai < pai->n && redistribui(pai, pos_pai, ap, pai->p[pos_pai+1], 1)) {
        *cresceu = 0;
        return;
    }
    if (ap->n == ORDEM_BSTAR - 1) {
        split_bstar(pai, pos_pai, ap, *reg_sobe, *novo_dir, reg_sobe, novo_dir);
        *cresceu = 1;
    } else {
        insere_na_pagina_estrela(ap, *reg_sobe, *novo_dir);
        *cresceu = 0;
    }
}

// Função principal de inserção
void insere_b_estrela(tipo_registro_estrela reg, tipo_apontador_estrela *arv) {
    int cresceu;
    tipo_registro_estrela reg_sobe;
    tipo_apontador_estrela novo_dir;
    if (*arv == NULL) {
        *arv = (tipo_apontador_estrela)malloc(sizeof(tipo_pagina_estrela));
        (*arv)->n = 1;
        (*arv)->r[0] = reg;
        for (int i = 0; i < ORDEM_BSTAR+1; i++) (*arv)->p[i] = NULL;
        return;
    }
    insere_bstar_rec(NULL, -1, reg, *arv, &cresceu, &reg_sobe, &novo_dir);
    if (cresceu) {
        tipo_apontador_estrela nova_raiz = (tipo_apontador_estrela)malloc(sizeof(tipo_pagina_estrela));
        nova_raiz->n = 1;
        nova_raiz->r[0] = reg_sobe;
        nova_raiz->p[0] = *arv;
        nova_raiz->p[1] = novo_dir;
        for (int i = 2; i < ORDEM_BSTAR+1; i++) nova_raiz->p[i] = NULL;
        *arv = nova_raiz;
        TRANSF_EXT_INT_BSTAR_PRE++; // Transferência ao criar nova raiz
    }
}

tipo_apontador_estrela contruir_arvore_b_estrela(const char *nomeArquivo, int quantidade) {
    FILE *fp = fopen(nomeArquivo, "rb");
    if (!fp) {
        printf("Erro ao abrir arquivo %s\n", nomeArquivo);
        return NULL;
    }


    tipo_apontador_estrela arvore;
    inicializa_b_estrela(&arvore);
    tipo_registro_estrela temp;

    for (int i = 0; i < quantidade; i++) {
        fread(&temp, sizeof(tipo_registro_estrela), 1, fp);
        insere_b_estrela(temp, &arvore);
    }
    fclose(fp);


    return arvore;
}

// Libera toda a memória da árvore B* (recursivo)
void libera_arvore_b_estrela(tipo_apontador_estrela arvore) {
    if (!arvore) return;
    for (int i = 0; i <= arvore->n; i++)
        libera_arvore_b_estrela(arvore->p[i]);
    free(arvore);
}

// Imprime os contadores de operações da árvore B*
void print_counters_bstar() {
    printf("Pré-processamento:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_BSTAR_PRE);
    printf("  Comparações: %d\n", COMP_CHAVES_BSTAR_PRE);
    printf("Pós-processamento:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_BSTAR_POS);
    printf("  Comparações: %d\n", COMP_CHAVES_BSTAR_POS);
    printf("Total:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_BSTAR_PRE + TRANSF_EXT_INT_BSTAR_POS);
    printf("  Comparações: %d\n", COMP_CHAVES_BSTAR_PRE + COMP_CHAVES_BSTAR_POS);
}
