#include "include/asi.h"
#include <stdlib.h>
#include <string.h>


void alloc_index_table(index_table_t* source, int tam) {
    source->table = (int*) malloc(sizeof(int) * (tam/ITENS_PAG));
    source->tam = (tam%ITENS_PAG != 0) ? tam/ITENS_PAG + 1 : tam/ITENS_PAG;
}

bool create_index_table(FILE *fp, index_table_t *source){
    if (fp == NULL)
        return false;

    pag_t pag;

    for (int i = 0; i < source->tam; i++) {
        if (fread(&pag, sizeof(reg_t), ITENS_PAG, fp) != 0) {
            source->table[i] = pag[0].chave;
        }
        else
            return false;
    }

    return true;
}

bool search(FILE *arq, char* ord, index_table_t table, reg_t *x){
    if (arq == NULL)
        return false;

    pag_t pag;
    int pag_index, qnt_itens;
    long desloc;

    pag_index = 0;

    if (strcmp(ord, "asc") == 0)
        while (pag_index < table.tam && table.table[pag_index] <= x->chave ) pag_index++;
    else if (strcmp(ord, "desc") == 0)
        while (pag_index < table.tam && table.table[pag_index] >= x->chave ) pag_index++;
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

    fread(&pag, sizeof(reg_t), qnt_itens, arq);

    for (int i = 0; i < qnt_itens; i++) {
        if (pag[i].chave == x->chave)
        {
            *x = pag[i]; return true;
        }
    }

    return false;
}
