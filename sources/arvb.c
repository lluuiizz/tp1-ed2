#include "../include/arvb.h"
#include <stdlib.h>

// Contadores para pré-processamento (construção da árvore)
int TRANSF_EXT_INT_ARVB_PRE = 0; // Transferências de páginas durante a construção
int COMP_CHAVES_ARVB_PRE = 0;    // Comparações de chaves durante a construção

// Contadores para pós-processamento (busca)
int TRANSF_EXT_INT_ARVB_POS = 0; // Transferências de páginas durante a busca
int COMP_CHAVES_ARVB_POS = 0;    // Comparações de chaves durante a busca
void Inicializa (TipoApontador Arvore)
{
Arvore = NULL;
}

void Pesquisa(TipoRegistro *x, TipoApontador Ap)
{
    long i = 1;

    if (Ap == NULL)
    {
        printf("TipoRegistro nao esta presente na arvore\n");
        return;
    }

    // Pesquisa sequencial para se encontrar o intervalo desejado
    while (i < Ap->n && x->Chave > Ap->r[i-1].Chave) i++;

    // Verifica se a chave desejada foi localizada
    if (x->Chave == Ap->r[i-1].Chave)
    {
        *x = Ap->r[i-1];
        return;
    }

    // Ativacao recursiva da Pesquisa em uma das subarvores (esquerda ou direita)
    if (x->Chave < Ap->r[i-1].Chave)
        Pesquisa(x, Ap->p[i-1]);
    else
        Pesquisa(x, Ap->p[i]);
}

void Imprime(TipoApontador arvore){
int i = 0;
    if (arvore == NULL) return;
    while (i <= arvore->n) {
        Imprime(arvore->p[i]);
    if (i != arvore->n)
        printf(arvore->r[i].Chave + " ");
        i++;
    }
}

void InsereNaPagina(TipoApontador Ap, TipoRegistro Reg, TipoApontador ApDir)
{
    short NaoAchouPosicao;
    int k;

    k = Ap->n;
    NaoAchouPosicao = (k > 0);

    while (NaoAchouPosicao)
    {
        if (Reg.Chave >= Ap->r[k-1].Chave)
        {
            NaoAchouPosicao = FALSE;
            break;
        }

        Ap->r[k] = Ap->r[k-1];
        Ap->p[k+1] = Ap->p[k];
        k--;

        if (k < 1)
            NaoAchouPosicao = FALSE;
    }
    
    Ap->r[k] = Reg;
    Ap->p[k+1] = ApDir;
    Ap->n++;
}

void Ins(TipoRegistro Reg, TipoApontador Ap, short *Cresceu,
         TipoRegistro *RegRetorno, TipoApontador *ApRetorno)
{
    long i = 1;
    long j;
    TipoApontador ApTemp;

    // Caso base: se a página é nula, a inserção é aqui.
    // O registro e o apontador de retorno são definidos, e a flag 'Cresceu' é ativada.
    if (Ap == NULL)
    {
        *Cresceu = TRUE; 
        (*RegRetorno) = Reg; 
        (*ApRetorno) = NULL;
        return;
    }

    // Pesquisa o intervalo para o registro na página atual
    while (i < Ap->n && Reg.Chave > Ap->r[i-1].Chave)
        i++;
    
    // Se a chave já existe, retorna um erro
    if (Reg.Chave == Ap->r[i-1].Chave)
    {
        printf(" Erro: Registro ja esta presente\n");
        *Cresceu = FALSE;
        return;
    }

    // Ajusta o índice 'i' se a chave for menor que a chave na posição i-1
    if (Reg.Chave < Ap->r[i-1].Chave)
        i--;
    
    // Chamada recursiva para inserir na subárvore apropriada
    Ins(Reg, Ap->p[i], Cresceu, RegRetorno, ApRetorno);

    // Se a árvore não cresceu (ou seja, a inserção foi feita sem divisão), retorna.
    if (!*Cresceu)
        return;
    
    // Se a página tem espaço, insere o registro e o apontador de retorno
    if (Ap->n < MM) /* Pagina tem espaco */
    {
        InsereNaPagina(Ap, *RegRetorno, *ApRetorno);
        *Cresceu = FALSE; // A propagação de crescimento para.
        return;
    }

    // Se a página está cheia (overflow), ela precisa ser dividida
    /* Overflow: Pagina tem que ser dividida */
    ApTemp = (TipoApontador)malloc(sizeof(TipoPagina));
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
        InsereNaPagina(ApTemp, Ap->r[MM - 1], Ap->p[MM]);
        Ap->n--;
        InsereNaPagina(Ap, *RegRetorno, *ApRetorno);
    }
    else
    {
        // Se a inserção ocorre na metade direita da página
        InsereNaPagina(ApTemp, *RegRetorno, *ApRetorno);
    }

    // Distribui os registros da página original para a nova página criada
    for (j = M + 2; j <= MM; j++)
        InsereNaPagina(ApTemp, Ap->r[j - 1], Ap->p[j]);
    
    // Atualiza o número de registros na página original
    Ap->n = M;
    
    // Atualiza o primeiro apontador da nova página
    ApTemp->p[0] = Ap->p[M + 1];
    
    // O registro do meio é promovido para a página pai
    *RegRetorno = Ap->r[M];
    
    // O apontador para a nova página é retornado
    *ApRetorno = ApTemp;
}
void Insere(TipoRegistro Reg, TipoApontador *Ap)
{
    short Cresceu;
    TipoRegistro RegRetorno;
    TipoPagina *ApRetorno, *ApTemp;

    // Inicia a inserção recursiva
    Ins(Reg, *Ap, &Cresceu, &RegRetorno, &ApRetorno);
    
    // Se a flag 'Cresceu' está ativada, a raiz precisa ser dividida.
    if (Cresceu) /* Arvore cresce na altura pela raiz */
    {
        // Cria uma nova raiz
        ApTemp = (TipoPagina *)malloc(sizeof(TipoPagina));
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
