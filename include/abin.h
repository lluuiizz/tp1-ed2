#ifndef __ABIN_H__
#define __ABIN_H__

#include "reg.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    int left_node;
    reg_t reg;
    int right_node;

} reg_abin_t;

bool create_binary_tree(FILE *source, int qnt_regs);
bool binary_search(FILE *source, reg_t *x);
void print_counters_abin();
void exibir_reg_abin(FILE *fp, int quantidade);
#endif // __ABIN_H__
