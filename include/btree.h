#ifndef __BTREE_H__
#define __BTREE_H__

#include "reg.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define M 2
#define MM 2*M

typedef struct pag* pag_pointer_t;

typedef struct pag {
    reg_t itens[MM]; short n;      // Itens of page P and his number of elements

    pag_pointer_t pag_ps[MM + 1]; // Pointers to descendents of page P

} pag_t;

void init_tree(pag_pointer_t tree);
bool btree_search(reg_t *x, pag_pointer_t tree);
#endif //__BTREE_H__
