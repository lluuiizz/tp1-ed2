#ifndef __ARVB_H__
#define __ARVB_H__

#include <stdio.h>
#include <stdbool.h>
#include "reg.h"
#define ORDEM 1000

typedef struct tipo_pagina* tipo_apontador;

typedef struct {
    int chave;
    long dado1;
    char dado2[1000];
    char dado3[5000];
} tipo_registro;

struct tipo_pagina {
    int n;
    tipo_registro r[ORDEM];
    tipo_apontador p[ORDEM+1];
};

// Funções da Árvore B
void inicializa(tipo_apontador *arvore);
void insere(tipo_registro reg, tipo_apontador *ap);
bool pesquisa_arvore_b(tipo_registro *x, tipo_apontador ap);
void exibir_reg_arvb(tipo_apontador arvore);
void libera_arvore_b(tipo_apontador arvore);
tipo_apontador construir_arvore_b(const char *nomeArquivo, int quantidade);
void print_counters_arvb();
#endif // __ARVB_H__
