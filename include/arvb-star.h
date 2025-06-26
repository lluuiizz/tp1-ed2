#ifndef __ARVB_ESTRELA_H__
#define __ARVB_ESTRELA_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "pag.h"

#define M2 2
#define MM2 (2*M2)


typedef long TipoChave;

typedef struct {
    TipoChave Chave;
    long dado1;
    char dado2[1000];
    char dado3[5000];
} TipoRegistro;

typedef enum { Interna, Externa } TipoIntExt;

typedef struct TipoPagina* TipoApontador;

// === ESTRUTURA CORRIGIDA AQUI ===
typedef struct TipoPagina {
    TipoIntExt Pt;
    union {
        struct {
            int ni;
            TipoChave ri[MM];
            TipoApontador pi[MM + 1];
        } U0;
        struct {
            int ne;
            TipoRegistro re[MM2];
        } U1;
    } UU;
} TipoPagina;
// Funções de Uso Externo

void InicializaBStar(TipoApontador *);
bool PesquisaBStar(TipoRegistro* x, TipoApontador Ap);
void ImprimeBStar(TipoApontador Arvore, int nivel);
void LiberaBStar(TipoApontador Ap);
void ConstroiArvoreDoArquivo(TipoApontador* Arvore, const char* nome_arquivo, int quantidade);
void print_counters_bstar();





#endif // __ARVB_ESTRELA_H__
