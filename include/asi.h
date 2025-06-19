#ifndef __ASI_H__
#define __ASI_H__

#include "reg.h"
#include <stdio.h>
#include <stdbool.h>

#define ITENS_PAG 4

typedef struct {
    int *table;
    int tam;
} index_table_t;

typedef reg_t pag_t[ITENS_PAG]; //  Pagina

// Tam é a quantidade de registros no arquivo
// Como cada página guardara 4 registros então
// Teremos um tamanho da tabela = Registros/4
// Pois guardara apenas a chave do primeiro registro
// De cada página
void alloc_index_table(index_table_t* source, int tam);
bool create_index_table(FILE* fp, index_table_t *source);
bool search(FILE *arq, char* ord, index_table_t, reg_t *x);

#endif // __ASI_H__
