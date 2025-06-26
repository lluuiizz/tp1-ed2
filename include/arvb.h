#ifndef __ARVB_H__
#define __ARVB_H__

#include <stdio.h>
#include <stdbool.h>
#include "pag.h"

// Funções da Árvore B
bool pesquisa_arvore_b(tipo_registro *, tipo_apontador);
void libera_arvore_b(tipo_apontador arvore);
tipo_apontador construir_arvore_b(const char *nomeArquivo, int quantidade);
void print_counters_arvb();
#endif // __ARVB_H__
