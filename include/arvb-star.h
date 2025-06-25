#ifndef __ARVB_ESTRELA_H__
#define __ARVB_ESTRELA_H__

#include <stdio.h>
#include <stdbool.h>

#define ORDEM_BSTAR 1000

typedef struct {
    int chave;
} tipo_registro_estrela;

typedef struct tipo_pagina_estrela* tipo_apontador_estrela;

typedef struct tipo_pagina_estrela {
    int n;
    tipo_registro_estrela r[ORDEM_BSTAR];
    tipo_apontador_estrela p[ORDEM_BSTAR+1];
} tipo_pagina_estrela;

// Funções da Árvore B*
void inicializa_b_estrela(tipo_apontador_estrela *arvore);
bool pesquisa_b_estrela(tipo_registro_estrela *x, tipo_apontador_estrela ap);
void exibir_reg_arvbstar(tipo_apontador_estrela arvore);
void insere_b_estrela(tipo_registro_estrela reg, tipo_apontador_estrela *ap);
void libera_arvore_b_estrela(tipo_apontador_estrela arvore);
void print_counters_bstar();

#endif // __ARVB_ESTRELA_H__