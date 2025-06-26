#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Definições para a ordem da árvore
// M é o número mínimo de chaves em uma página interna (M <= ni <= 2*M)
// MM é o número máximo de chaves em uma página interna (MM = 2 * M)
#define M 2
#define MM (2 * M)
#define NE (2 * M) // Número máximo de elementos em uma página folha
#define ME (M)     // Número mínimo de elementos em uma página folha

// Tipos de página
typedef enum {
    Interna,
    Externa
} TipoPagina;

// Estrutura para os registros de dados
typedef struct {
    int Chave;
} Registro;

// Estrutura para uma página da árvore
typedef struct TipoPagina_ *TipoApontador;
typedef struct TipoPagina_ {
    TipoPagina Pt; // Tipo de página (Interna ou Externa)
    union {
        // Estrutura para páginas internas (nós de índice)
        struct {
            int ni; // Número de chaves
            Registro ri[MM]; // Chaves do nó
            TipoApontador pi[MM + 1]; // Ponteiros para as subárvores
        } U0;
        // Estrutura para páginas folha (registros de dados)
        struct {
            int ne; // Número de registros
            Registro re[NE]; // Registros de dados
            TipoApontador proximo; // Ponteiro para a próxima página folha
        } U1;
    } UU;
} TipoPagina_;

// Protótipos das funções
void CriaPagina(TipoPagina tipo, TipoApontador *Ap);
void Inicializa(TipoApontador *Dicionario);
void ImprimeBStar(TipoApontador Arvore, int nivel);
void Pesquisa(Registro *x, TipoApontador Ap);
void InsereNaPagina(TipoApontador Ap, Registro Reg, TipoApontador ApDir);
void InsBStar(Registro Reg, TipoApontador *Ap, bool *Cresceu, Registro *RegRetorno, TipoApontador *ApRetorno);
void Insere(Registro Reg, TipoApontador *Ap);
void RemoveDaPagina(TipoApontador Ap, Registro Reg);
void RemoveDaPaginaInterna(TipoApontador Ap, int Pos);
void Rem(Registro Reg, TipoApontador Ap, bool *Diminuiu);
void Remove(Registro Reg, TipoApontador *Ap);
void liberaArvore(TipoApontador Arvore);
void EncontraSucessor(TipoApontador Ap, Registro *RegSucessor, TipoApontador *PaginaSucessor);

//----------------------------------------------------------------------------------------------------------------------
// Funções de Gerenciamento da Árvore
//----------------------------------------------------------------------------------------------------------------------

/**
 * @brief Cria uma nova página (Interna ou Externa) e a inicializa.
 * @param tipo O tipo de página a ser criada.
 * @param Ap Ponteiro para o ponteiro que apontará para a nova página.
 */
void CriaPagina(TipoPagina tipo, TipoApontador *Ap) {
    *Ap = (TipoApontador)malloc(sizeof(TipoPagina_));
    if (*Ap == NULL) {
        perror("Erro ao alocar memoria para a pagina");
        exit(EXIT_FAILURE);
    }
    (*Ap)->Pt = tipo;
    if (tipo == Interna) {
        (*Ap)->UU.U0.ni = 0;
    } else { // Externa
        (*Ap)->UU.U1.ne = 0;
        (*Ap)->UU.U1.proximo = NULL;
    }
}

/**
 * @brief Inicializa a árvore, criando uma página raiz vazia.
 * @param Dicionario Ponteiro para a raiz da árvore.
 */
void Inicializa(TipoApontador *Dicionario) {
    *Dicionario = NULL;
}

/**
 * @brief Libera a memória alocada por toda a árvore de forma recursiva.
 * @param Arvore A raiz da árvore a ser liberada.
 */
void liberaArvore(TipoApontador Arvore) {
    if (Arvore == NULL) {
        return;
    }

    if (Arvore->Pt == Interna) {
        for (int i = 0; i <= Arvore->UU.U0.ni; i++) {
            liberaArvore(Arvore->UU.U0.pi[i]);
        }
    }

    free(Arvore);
}

/**
 * @brief Imprime a estrutura da árvore B* de forma formatada.
 * @param Arvore O apontador para o nó a ser impresso.
 * @param nivel O nível de indentação para a impressão.
 */
void ImprimeBStar(TipoApontador Arvore, int nivel) {
    if (Arvore == NULL) {
        return;
    }

    // Indentação para a hierarquia
    for (int i = 0; i < nivel; i++) {
        printf("   ");
    }

    if (Arvore->Pt == Interna) {
        printf("[Index]: ");
        for (int i = 0; i < Arvore->UU.U0.ni; i++) {
            printf("%d ", Arvore->UU.U0.ri[i].Chave);
        }
        printf("\n");
        // Chama a função recursivamente para os filhos
        for (int i = 0; i <= Arvore->UU.U0.ni; i++) {
            ImprimeBStar(Arvore->UU.U0.pi[i], nivel + 1);
        }
    } else { // Pt == Externa
        printf("[Folha]: ");
        for (int i = 0; i < Arvore->UU.U1.ne; i++) {
            printf("%d ", Arvore->UU.U1.re[i].Chave);
        }
        printf("\n");
    }
}

/**
 * @brief Procura por um registro na árvore.
 * @param x O registro a ser pesquisado.
 * @param Ap O apontador para a página atual.
 */
void Pesquisa(Registro *x, TipoApontador Ap) {
    int i;
    if (Ap == NULL) {
        printf("Chave %d nao encontrada.\n", x->Chave);
        return;
    }

    if (Ap->Pt == Interna) {
        i = 0;
        // Pesquisa sequencial na página interna
        while (i < Ap->UU.U0.ni && x->Chave >= Ap->UU.U0.ri[i].Chave) {
            i++;
        }
        Pesquisa(x, Ap->UU.U0.pi[i]);
    } else { // Pt == Externa
        i = 0;
        // Pesquisa sequencial na página folha
        while (i < Ap->UU.U1.ne && x->Chave > Ap->UU.U1.re[i].Chave) {
            i++;
        }
        if (i < Ap->UU.U1.ne && x->Chave == Ap->UU.U1.re[i].Chave) {
            // printf("Chave %d encontrada.\n", x->Chave);
        } else {
            // printf("Chave %d nao encontrada.\n", x->Chave);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Funções de Inserção
//----------------------------------------------------------------------------------------------------------------------

/**
 * @brief Insere um registro e um ponteiro em uma página interna.
 * @param Ap A página interna.
 * @param Reg O registro a ser inserido.
 * @param ApDir O ponteiro para a subárvore à direita do registro.
 */
void InsereNaPagina(TipoApontador Ap, Registro Reg, TipoApontador ApDir) {
    int k = Ap->UU.U0.ni;
    while (k > 0 && Reg.Chave < Ap->UU.U0.ri[k - 1].Chave) {
        Ap->UU.U0.ri[k] = Ap->UU.U0.ri[k - 1];
        Ap->UU.U0.pi[k + 1] = Ap->UU.U0.pi[k];
        k--;
    }
    Ap->UU.U0.ri[k] = Reg;
    Ap->UU.U0.pi[k + 1] = ApDir;
    Ap->UU.U0.ni++;
    printf("DEBUG: InsereNaPagina: Inserido chave %d e ponteiro %p na pos %d do no interno %p. Novo ni: %d\n",
           Reg.Chave, ApDir, k, Ap, Ap->UU.U0.ni);
}

/**
 * @brief Função recursiva para inserção em páginas internas ou folhas.
 * @param Reg O registro a ser inserido.
 * @param Ap Ponteiro para o apontador da página atual.
 * @param Cresceu Ponteiro para um booleano que indica se a árvore cresceu.
 * @param RegRetorno Registro a ser promovido para o nível superior.
 * @param ApRetorno Apontador para a nova página criada pelo split.
 */
void InsBStar(Registro Reg, TipoApontador *Ap, bool *Cresceu, Registro *RegRetorno, TipoApontador *ApRetorno) {
    int i = 0;
    
    // CASO BASE: Árvore vazia (raiz)
    if (*Ap == NULL) {
        *Cresceu = true;
        *ApRetorno = NULL;
        *RegRetorno = Reg;
        return;
    }
    
    // Desce pela árvore até a página folha
    if ((*Ap)->Pt == Interna) {
        while (i < (*Ap)->UU.U0.ni && Reg.Chave >= (*Ap)->UU.U0.ri[i].Chave) {
            i++;
        }
        InsBStar(Reg, &(*Ap)->UU.U0.pi[i], Cresceu, RegRetorno, ApRetorno);

        if (!(*Cresceu)) return; // Se não cresceu, sobe sem fazer nada

        // Se a página filha cresceu, insere a chave promovida nesta página interna
        if ((*Ap)->UU.U0.ni < MM) {
            InsereNaPagina(*Ap, *RegRetorno, *ApRetorno);
            *Cresceu = false;
            return;
        }

        // Se a página interna está cheia, precisa dividir (split)
        // Lógica de split para nó interno
        TipoApontador ApTemp;
        CriaPagina(Interna, &ApTemp);
        Registro tempChaves[MM + 1];
        TipoApontador tempPonteiros[MM + 2];
        
        // Copia as chaves e ponteiros do nó atual para os arrays temporários
        for (int k = 0; k < (*Ap)->UU.U0.ni; k++) {
            tempChaves[k] = (*Ap)->UU.U0.ri[k];
            tempPonteiros[k] = (*Ap)->UU.U0.pi[k];
        }
        tempPonteiros[(*Ap)->UU.U0.ni] = (*Ap)->UU.U0.pi[(*Ap)->UU.U0.ni];

        // Insere a nova chave e ponteiro no array temporário
        int k = (*Ap)->UU.U0.ni;
        while (k > 0 && RegRetorno->Chave < tempChaves[k - 1].Chave) {
            tempChaves[k] = tempChaves[k - 1];
            tempPonteiros[k + 1] = tempPonteiros[k];
            k--;
        }
        tempChaves[k] = *RegRetorno;
        tempPonteiros[k + 1] = *ApRetorno;

        // Limpa o nó original e distribui as chaves e ponteiros
        (*Ap)->UU.U0.ni = 0;
        int j = 0;
        for (int l = 0; l < M; l++) {
            (*Ap)->UU.U0.ri[l] = tempChaves[l];
            (*Ap)->UU.U0.pi[l] = tempPonteiros[l];
            (*Ap)->UU.U0.ni++;
            j++;
        }
        (*Ap)->UU.U0.pi[j] = tempPonteiros[j];
        
        // Promove a chave do meio para o nó pai
        *RegRetorno = tempChaves[M];
        
        // Move as chaves e ponteiros restantes para a nova página
        ApTemp->UU.U0.ni = 0;
        j = 0;
        for (int l = M + 1; l <= MM; l++) {
            ApTemp->UU.U0.ri[j] = tempChaves[l];
            ApTemp->UU.U0.pi[j] = tempPonteiros[l];
            ApTemp->UU.U0.ni++;
            j++;
        }
        ApTemp->UU.U0.pi[j] = tempPonteiros[MM + 1];

        *ApRetorno = ApTemp;
        *Cresceu = true;
        printf("DEBUG: Split de no interno %p. Chave promovida: %d. Novo no %p. Antigo ni: %d, Novo ni: %d\n",
               *Ap, RegRetorno->Chave, *ApRetorno, (*Ap)->UU.U0.ni, ApTemp->UU.U0.ni);
        
    } else { // Pt == Externa (página folha)
        // Tenta inserir na página folha
        if ((*Ap)->UU.U1.ne < NE) {
            int i = (*Ap)->UU.U1.ne;
            while (i > 0 && Reg.Chave < (*Ap)->UU.U1.re[i - 1].Chave) {
                (*Ap)->UU.U1.re[i] = (*Ap)->UU.U1.re[i - 1];
                i--;
            }
            (*Ap)->UU.U1.re[i] = Reg;
            (*Ap)->UU.U1.ne++;
            *Cresceu = false; // Não cresceu, pois coube na página
            return;
        }

        // Se a página folha está cheia, precisa dividir (split)
        TipoApontador ApTemp;
        CriaPagina(Externa, &ApTemp);

        // Copia o ponteiro para o próximo nó folha
        ApTemp->UU.U1.proximo = (*Ap)->UU.U1.proximo;
        (*Ap)->UU.U1.proximo = ApTemp;

        // Array temporário para as chaves
        Registro tempReg[NE + 1];
        for (int k = 0; k < NE; k++) {
            tempReg[k] = (*Ap)->UU.U1.re[k];
        }

        // Insere a nova chave no array temporário
        int k = NE;
        while (k > 0 && Reg.Chave < tempReg[k - 1].Chave) {
            tempReg[k] = tempReg[k - 1];
            k--;
        }
        tempReg[k] = Reg;

        // Limpa a página original
        (*Ap)->UU.U1.ne = 0;
        // Distribui as chaves entre a página original e a nova
        for (int l = 0; l < M; l++) {
            (*Ap)->UU.U1.re[l] = tempReg[l];
            (*Ap)->UU.U1.ne++;
        }
        
        ApTemp->UU.U1.ne = 0;
        for (int l = M; l <= NE; l++) {
            ApTemp->UU.U1.re[l - M] = tempReg[l];
            ApTemp->UU.U1.ne++;
        }

        // A chave que sobe para o pai é a primeira da nova página
        *RegRetorno = ApTemp->UU.U1.re[0];
        *ApRetorno = ApTemp;
        *Cresceu = true;
    }
}

/**
 * @brief Função principal de inserção.
 * @param Reg O registro a ser inserido.
 * @param Ap Ponteiro para a raiz da árvore.
 */
void Insere(Registro Reg, TipoApontador *Ap) {
    bool Cresceu;
    Registro RegRetorno;
    TipoApontador ApRetorno;
    
    InsBStar(Reg, Ap, &Cresceu, &RegRetorno, &ApRetorno);

    //  Cria uma nova raiz se a árvore cresceu no nível superior (split da raiz)
    if (Cresceu) {
        TipoApontador ApTemp;
        CriaPagina(Interna, &ApTemp);
        ApTemp->UU.U0.ri[0] = RegRetorno;
        ApTemp->UU.U0.pi[0] = *Ap;
        ApTemp->UU.U0.pi[1] = ApRetorno;
        ApTemp->UU.U0.ni = 1;
        *Ap = ApTemp;
        printf("DEBUG: Nova raiz criada (%p). Cresceu no nivel superior.\n", *Ap);
    }
}
