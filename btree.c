#include "include/btree.h"

void init_tree(pag_pointer_t tree){
    tree = NULL;
}


bool btree_search(reg_t *x, pag_pointer_t tree){
    bool encountered = false;

    if (tree == NULL) return encountered;

    long i = 1;
    int key_to_found = x->chave;
    while (i < tree->n && key_to_found > tree->itens[i-1].chave) i++;

    if (key_to_found == tree->itens[i-1].chave) {
        *x  = tree->itens[i-1];
        encountered = true;
        return encountered;
    }

    if (key_to_found < tree->itens[i-1].chave)
        btree_search(x, tree->pag_ps[i-1]);
    else
        btree_search(x, tree->pag_ps[i]);

    return encountered;
}

void print_btree(pag_pointer_t tree) {
    int i = 0;

    if (tree == NULL) return;

    while (i <= tree->n) {
        print_btree(tree->pag_ps[i]);

        if (i != tree->n)
            printf("Chave: %d\n", tree->itens[i].chave);
        i++;

    }

}
