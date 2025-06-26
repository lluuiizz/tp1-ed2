#include "../include/arvb-star.h"
#include <stdlib.h>

// Contadores para pré-processamento (construção da árvore B*)
int TRANSF_EXT_INT_BSTAR_PRE = 0;
int COMP_CHAVES_BSTAR_PRE = 0;

// Contadores para pós-processamento (busca)
int TRANSF_EXT_INT_BSTAR_POS = 0;
int COMP_CHAVES_BSTAR_POS = 0;

// Inicializa a árvore B* (define ponteiro raiz como NULL)


// Imprime os contadores de operações da árvore B*
void print_counters_bstar() {
    printf("Pré-processamento:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_BSTAR_PRE);
    printf("  Comparações: %d\n", COMP_CHAVES_BSTAR_PRE);
    printf("Pós-processamento:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_BSTAR_POS);
    printf("  Comparações: %d\n", COMP_CHAVES_BSTAR_POS);
    printf("Total:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_BSTAR_PRE + TRANSF_EXT_INT_BSTAR_POS);
    printf("  Comparações: %d\n", COMP_CHAVES_BSTAR_PRE + COMP_CHAVES_BSTAR_POS);
}
