#include "include/asi.h"
#include <stdlib.h>
#include <string.h>

// Contadores para pré-processamento (criação da tabela de índices)
int TRANSF_EXT_INT_ASI_PRE = 0;
int COMP_CHAVES_ASI_PRE = 0;

// Contadores para pós-processamento (busca)
int TRANSF_EXT_INT_ASI_POS = 0;
int COMP_CHAVES_ASI_POS = 0;

// Aloca a tabela de índices para o arquivo, baseado no tamanho total de registros
void alloc_index_table(index_table_t* source, int tam) {
    source->table = (int*) malloc(sizeof(int) * (tam/ITENS_PAG));
    source->tam = (tam%ITENS_PAG != 0) ? tam/ITENS_PAG + 1 : tam/ITENS_PAG;
}

// Cria a tabela de índices lendo o primeiro elemento de cada página do arquivo
bool create_index_table(FILE *fp, index_table_t *source){
    if (fp == NULL)
        return false;

    pag_t pag;

    for (int i = 0; i < source->tam; i++) {
        size_t lidos = fread(&pag, sizeof(reg_t), ITENS_PAG, fp);
        TRANSF_EXT_INT_ASI_PRE += lidos; // Conta transferências de itens (pré)
        if (lidos != 0) {
            source->table[i] = pag[0].chave;
        }
        else
            return false;
    }

    return true;
}

// Busca um registro no arquivo usando a tabela de índices
bool search(FILE *arq, char* ord, index_table_t table, reg_t *x){
    if (arq == NULL)
        return false;

    pag_t pag;
    int pag_index, qnt_itens;
    long desloc;

    pag_index = 0;

    // Busca a página onde o registro pode estar, de acordo com a ordem (ascendente ou descendente)
    if (strcmp(ord, "asc") == 0)
        while (pag_index < table.tam && (COMP_CHAVES_ASI_POS++, table.table[pag_index] <= x->chave)) pag_index++;
    else if (strcmp(ord, "desc") == 0)
        while (pag_index < table.tam && (COMP_CHAVES_ASI_POS++, table.table[pag_index] >= x->chave)) pag_index++;
    else
        return false;

    if (pag_index == 0) return false;
    else pag_index--;

    if (pag_index < (table.tam-1)) qnt_itens = ITENS_PAG;
    else {
        fseek(arq, 0, SEEK_END);
        qnt_itens = ((ftell(arq) / sizeof(reg_t)) % ITENS_PAG);
        qnt_itens = (qnt_itens == 0) ? ITENS_PAG : qnt_itens;
    }

    desloc = pag_index * sizeof(reg_t) * ITENS_PAG;
    fseek(arq, desloc, SEEK_SET);

    size_t lidos = fread(&pag, sizeof(reg_t), qnt_itens, arq);
    TRANSF_EXT_INT_ASI_POS += lidos; // Conta transferências de itens (pós)

    for (int i = 0; i < qnt_itens; i++) {
        COMP_CHAVES_ASI_POS++; // Conta comparação de chave (pós)
        if (pag[i].chave == x->chave)
        {
            *x = pag[i]; return true;
        }
    }

    return false;
}

void exibir_reg_asi(FILE *fp, int quantidade, index_table_t *table) {
    reg_t temp;
    fseek(fp, 0, SEEK_SET);
    printf("Chaves dos registros:\n");
    for (int i = 0; i < quantidade; i++) {
        fread(&temp, sizeof(reg_t), 1, fp);
        printf("%d ", temp.chave);
    }
    printf("\n");
    fseek(fp, 0, SEEK_SET); // Volta para o início para a busca
}

void print_counters_asi() {
    printf("Pré-processamento:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_ASI_PRE);
    printf("  Comparações: %d\n", COMP_CHAVES_ASI_PRE);
    printf("Pós-processamento:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_ASI_POS);
    printf("  Comparações: %d\n", COMP_CHAVES_ASI_POS);
    printf("Total:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_ASI_PRE + TRANSF_EXT_INT_ASI_POS);
    printf("  Comparações: %d\n", COMP_CHAVES_ASI_PRE + COMP_CHAVES_ASI_POS);
}