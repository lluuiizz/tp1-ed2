#include "../include/arvb.h"
#include <stdlib.h>
#include <assert.h>

// Contadores para pré-processamento (construção da árvore)
int TRANSF_EXT_INT_ARVB_PRE = 0; // Transferências de páginas durante a construção
int COMP_CHAVES_ARVB_PRE = 0;    // Comparações de chaves durante a construção

// Contadores para pós-processamento (busca)
int TRANSF_EXT_INT_ARVB_POS = 0; // Transferências de páginas durante a busca
int COMP_CHAVES_ARVB_POS = 0;    // Comparações de chaves durante a busca



bool pesquisa_arvore_b(tipo_registro *x, tipo_apontador Ap)
{
    long i = 1;

    static bool returned_value = false;
    if (Ap == NULL)
    {
        return returned_value;
    }

    // Pesquisa sequencial para se encontrar o intervalo desejado
    while (i < Ap->n && x->chave > Ap->r[i-1].chave){COMP_CHAVES_ARVB_POS++; i++;}

    // Verifica se a chave desejada foi localizada

    if (x->chave == Ap->r[i-1].chave)
    {
        COMP_CHAVES_ARVB_POS++;
        *x = Ap->r[i-1];
        returned_value = true;
        return returned_value;
    }

    // Ativacao recursiva da Pesquisa em uma das subarvores (esquerda ou direita)
    if (x->chave < Ap->r[i-1].chave){
        COMP_CHAVES_ARVB_POS++;
        pesquisa_arvore_b(x, Ap->p[i-1]);
    }
    else{
        COMP_CHAVES_ARVB_POS++;
        pesquisa_arvore_b(x, Ap->p[i]);
    }

    return returned_value;
}

void insere_na_pag(tipo_apontador Ap, tipo_registro Reg, tipo_apontador ApDir)
{
    short NaoAchouPosicao;
    int k;

    k = Ap->n;
    NaoAchouPosicao = (k > 0);

    while (NaoAchouPosicao)
    {
        if (Reg.chave >= Ap->r[k-1].chave)
        {

            COMP_CHAVES_ARVB_PRE++;
            NaoAchouPosicao = false;
            break;
        }

        Ap->r[k] = Ap->r[k-1];
        Ap->p[k+1] = Ap->p[k];
        k--;

        if (k < 1)
            NaoAchouPosicao = false;
    }
    
    Ap->r[k] = Reg;

    Ap->p[k+1] = ApDir;
    Ap->n++;
}

void ins(tipo_registro Reg, tipo_apontador Ap, short *Cresceu,
         tipo_registro *RegRetorno, tipo_apontador *ApRetorno)
{
    long i = 1;
    long j;
    tipo_apontador ApTemp;

    // Caso base: se a página é nula, a inserção é aqui.
    // O registro e o apontador de retorno são definidos, e a flag 'Cresceu' é ativada.
    if (Ap == NULL)
    {
        *Cresceu = true;
        (*RegRetorno) = Reg; 
        (*ApRetorno) = NULL;
        return;
    }

    // Pesquisa o intervalo para o registro na página atual
    while (i < Ap->n && Reg.chave > Ap->r[i-1].chave){COMP_CHAVES_ARVB_PRE++; i++;}

    // Se a chave já existe, retorna um erro
    if (Reg.chave == Ap->r[i-1].chave)
    {
        COMP_CHAVES_ARVB_PRE++;
        *Cresceu = false;
        return;
    }

    // Ajusta o índice 'i' se a chave for menor que a chave na posição i-1
    if (Reg.chave < Ap->r[i-1].chave){
        COMP_CHAVES_ARVB_PRE++;
        i--;
    }

    // Chamada recursiva para inserir na subárvore apropriada
    ins(Reg, Ap->p[i], Cresceu, RegRetorno, ApRetorno);

    // Se a árvore não cresceu (ou seja, a inserção foi feita sem divisão), retorna.
    if (!*Cresceu)
        return;
    
    // Se a página tem espaço, insere o registro e o apontador de retorno
    if (Ap->n < MM) /* Pagina tem espaco */
    {
        insere_na_pag(Ap, *RegRetorno, *ApRetorno);
        *Cresceu = false; // A propagação de crescimento para.
        return;
    }

    // Se a página está cheia (overflow), ela precisa ser dividida
    /* Overflow: Pagina tem que ser dividida */
    ApTemp = (tipo_apontador)malloc(sizeof(tipo_pagina_t));
    if (ApTemp == NULL) {
        printf("Erro: Nao foi possivel alocar memoria para a nova pagina.\n");
        return;
    }
    ApTemp->n = 0;
    ApTemp->p[0] = NULL;

    // Decide qual parte da página será dividida
    if (i < M + 1)
    {
        // Se a inserção ocorre na metade esquerda da página
        insere_na_pag(ApTemp, Ap->r[MM - 1], Ap->p[MM]);
        Ap->n--;
        insere_na_pag(Ap, *RegRetorno, *ApRetorno);
    }
    else
    {
        // Se a inserção ocorre na metade direita da página
        insere_na_pag(ApTemp, *RegRetorno, *ApRetorno);
    }

    // Distribui os registros da página original para a nova página criada
    for (j = M + 2; j <= MM; j++)
        insere_na_pag(ApTemp, Ap->r[j - 1], Ap->p[j]);
    
    // Atualiza o número de registros na página original
    Ap->n = M;
    
    // Atualiza o primeiro apontador da nova página
    ApTemp->p[0] = Ap->p[M + 1];
    
    // O registro do meio é promovido para a página pai
    *RegRetorno = Ap->r[M];
    
    // O apontador para a nova página é retornado
    *ApRetorno = ApTemp;
}
void insere(tipo_registro Reg, tipo_apontador *Ap)
{
    short Cresceu;
    tipo_registro RegRetorno;
    tipo_apontador ApRetorno, ApTemp;

    // Inicia a inserção recursiva
    ins(Reg, *Ap, &Cresceu, &RegRetorno, &ApRetorno);
    
    // Se a flag 'Cresceu' está ativada, a raiz precisa ser dividida.
    if (Cresceu) /* Arvore cresce na altura pela raiz */
    {
        // Cria uma nova raiz
        ApTemp = (tipo_apontador)malloc(sizeof(tipo_pagina_t));
        if (ApTemp == NULL) {
            printf("Erro: Nao foi possivel alocar memoria para a nova raiz.\n");
            return;
        }
        
        ApTemp->n = 1;
        ApTemp->r[0] = RegRetorno;
        ApTemp->p[1] = ApRetorno;
        ApTemp->p[0] = *Ap;
        *Ap = ApTemp; // A nova página se torna a raiz da árvore
    }
}

tipo_apontador construir_arvore_b(const char *nomeArquivo, int quantidade){
    FILE *src = fopen(nomeArquivo, "rb");
    assert (src != NULL);
    tipo_apontador new_tree = (tipo_apontador) malloc(sizeof(tipo_pagina_t));

    assert(new_tree != NULL);
    tipo_registro new_reg;
    for (int i = 0; i < quantidade; i++) {
        TRANSF_EXT_INT_ARVB_PRE++;
        fread(&new_reg, sizeof(tipo_registro), 1, src);
        insere(new_reg, &new_tree);
    }

    fclose(src);
    return new_tree;

}


void libera_arvore_b(tipo_apontador arvore){

    if (arvore == NULL) return;

    for (int i = 0; i <= arvore->n; i++) {
        libera_arvore_b(arvore->p[i]);
    }

    free(arvore);
    arvore = NULL;
}
// Exibe os contadores de operações da árvore B
void print_counters_arvb() {
    printf("Pré-processamento:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_ARVB_PRE);
    printf("  Comparações: %d\n", COMP_CHAVES_ARVB_PRE);
    printf("Pós-processamento:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_ARVB_POS);
    printf("  Comparações: %d\n", COMP_CHAVES_ARVB_POS);
    printf("Total:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_ARVB_PRE + TRANSF_EXT_INT_ARVB_POS);
    printf("  Comparações: %d\n", COMP_CHAVES_ARVB_PRE + COMP_CHAVES_ARVB_POS);
}
