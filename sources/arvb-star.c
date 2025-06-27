#include "../include/arvb-star.h"
// Definições de constantes e tipos
// Contadores para pré-processamento (construção da árvore B*)

int TRANSF_EXT_INT_BSTAR_PRE = 0;
int COMP_CHAVES_BSTAR_PRE = 0;

// Contadores para pós-processamento (busca)
int TRANSF_EXT_INT_BSTAR_POS = 0;
int COMP_CHAVES_BSTAR_POS = 0;

// Inicializa a árvore B* (define ponteiro raiz como NULL)
void InicializaBStar(TipoApontador* Arvore) {
        *Arvore = NULL;
}

// Função de pesquisa
bool PesquisaBStar(TipoRegistro* x, TipoApontador Ap) {
    int i;
    if (Ap == NULL) {
        printf("Chave %ld nao encontrada\n", x->Chave);
        return false;
    }

    if (Ap->Pt == Interna) {
        i = 0;
        while (i < Ap->UU.U0.ni && x->Chave >= Ap->UU.U0.ri[i]) {
            COMP_CHAVES_BSTAR_POS++;
            i++;
        }
        return PesquisaBStar(x, Ap->UU.U0.pi[i]);
    }

    // Se a página for externa, procura o registro
    for (i = 0; i < Ap->UU.U1.ne; i++) {
        COMP_CHAVES_BSTAR_POS++;
        if (x->Chave == Ap->UU.U1.re[i].Chave) {
            *x = Ap->UU.U1.re[i];
            printf("Chave %ld encontrada\n", x->Chave);
            return true;
        }
    }
    return false;
}

// Funções de impressão para debug
void ImprimeBStar(TipoApontador Arvore, int nivel) {
    int i;
    if (Arvore == NULL) return;

    if (Arvore->Pt == Interna) {
        for (i = 0; i < nivel; i++) printf("    ");
        printf("[Indice]: ");
        for (i = 0; i < Arvore->UU.U0.ni; i++) printf("%ld ", Arvore->UU.U0.ri[i]);
        printf("\n");
        for (i = 0; i <= Arvore->UU.U0.ni; i++) ImprimeBStar(Arvore->UU.U0.pi[i], nivel + 1);
    } else {
        for (i = 0; i < nivel; i++) printf("    ");
        printf("[Folha]: ");
        for (i = 0; i < Arvore->UU.U1.ne; i++) printf("%ld ", Arvore->UU.U1.re[i].Chave);
        printf("\n");
    }
}

// Funções auxiliares de inserção
void InsereNaPaginaExterna(TipoApontador Ap, TipoRegistro Reg) {
    int i = Ap->UU.U1.ne - 1;
    while (i >= 0 && Reg.Chave < Ap->UU.U1.re[i].Chave) {
        TRANSF_EXT_INT_BSTAR_PRE++;
        Ap->UU.U1.re[i+1] = Ap->UU.U1.re[i];
        i--;
    }
    Ap->UU.U1.re[i+1] = Reg;
    Ap->UU.U1.ne++;
}

void InsereNaPaginaInterna(TipoApontador Ap, TipoChave Chave, TipoApontador Dir) {
    int k = Ap->UU.U0.ni;
    while (k > 0 && Chave < Ap->UU.U0.ri[k-1]) {
        TRANSF_EXT_INT_BSTAR_PRE++;
        Ap->UU.U0.ri[k] = Ap->UU.U0.ri[k-1];
        Ap->UU.U0.pi[k+1] = Ap->UU.U0.pi[k];
        k--;
    }
    Ap->UU.U0.ri[k] = Chave;
    Ap->UU.U0.pi[k+1] = Dir;
    Ap->UU.U0.ni++;
}

// Lógica de inserção recursiva
void InsBStar(TipoRegistro Reg, TipoApontador Ap, short* Cresceu,
              TipoRegistro* RegRetorno, TipoApontador* ApRetorno) {
    int i;
    TipoApontador ApTemp;

    if (Ap->Pt == Externa) {
        if (Ap->UU.U1.ne < MM2) {
            InsereNaPaginaExterna(Ap, Reg);
            *Cresceu = false;
            return;
        }

        ApTemp = (TipoApontador) malloc(sizeof(TipoPagina));
        if (ApTemp == NULL) { *Cresceu = false; return; }
        ApTemp->Pt = Externa;
        ApTemp->UU.U1.ne = 0;

        TipoRegistro temp[MM2 + 1];
        for (i = 0; i < MM2; i++) temp[i] = Ap->UU.U1.re[i];
        
        int pos_insercao = MM2;
        while (pos_insercao > 0 && Reg.Chave < temp[pos_insercao - 1].Chave) {
            COMP_CHAVES_BSTAR_PRE++;
            temp[pos_insercao] = temp[pos_insercao - 1];
            TRANSF_EXT_INT_BSTAR_PRE++;
            pos_insercao--;
        }
        temp[pos_insercao] = Reg;
        
        Ap->UU.U1.ne = 0;
        for (i = 0; i < M2; i++) {
            Ap->UU.U1.re[i] = temp[i];
            Ap->UU.U1.ne++;
        }
        for (i = M2; i <= MM2; i++) {
            ApTemp->UU.U1.re[ApTemp->UU.U1.ne++] = temp[i];
        }

        *RegRetorno = ApTemp->UU.U1.re[0];
        *ApRetorno = ApTemp;
        *Cresceu = true;
        return;
    }

    i = 0;
    while (i < Ap->UU.U0.ni && Reg.Chave >= Ap->UU.U0.ri[i]) {
        COMP_CHAVES_BSTAR_PRE++;
        i++;
    }

    InsBStar(Reg, Ap->UU.U0.pi[i], Cresceu, RegRetorno, ApRetorno);
    if (!*Cresceu) return;

    if (Ap->UU.U0.ni < MM) {
        InsereNaPaginaInterna(Ap, RegRetorno->Chave, *ApRetorno);
        *Cresceu = false;
        return;
    }

    ApTemp = (TipoApontador) malloc(sizeof(TipoPagina));
    if (ApTemp == NULL) { *Cresceu = false; return; }
    ApTemp->Pt = Interna;
    ApTemp->UU.U0.ni = 0;

    TipoChave tempChaves[MM+1];
    TipoApontador tempPonteiros[MM+2];
    for (i = 0; i < MM; i++) {
        tempChaves[i] = Ap->UU.U0.ri[i];
        tempPonteiros[i] = Ap->UU.U0.pi[i];
    }
    tempPonteiros[MM] = Ap->UU.U0.pi[MM];

    i = MM;
    while (i > 0 && RegRetorno->Chave < tempChaves[i-1]) {
        COMP_CHAVES_BSTAR_PRE++;
        tempChaves[i] = tempChaves[i-1];
        tempPonteiros[i+1] = tempPonteiros[i];
        TRANSF_EXT_INT_BSTAR_PRE++;
        i--;
    }
    tempChaves[i] = RegRetorno->Chave;
    tempPonteiros[i+1] = *ApRetorno;

    Ap->UU.U0.ni = 0;
    for (i = 0; i < M; i++) {
        Ap->UU.U0.ri[i] = tempChaves[i];
        Ap->UU.U0.pi[i] = tempPonteiros[i];
        Ap->UU.U0.ni++;
    }
    Ap->UU.U0.pi[i] = tempPonteiros[i];

    RegRetorno->Chave = tempChaves[M]; // Linha corrigida
    
    for (int j = M + 1; j <= MM; j++) {
        ApTemp->UU.U0.ri[ApTemp->UU.U0.ni] = tempChaves[j];
        ApTemp->UU.U0.pi[ApTemp->UU.U0.ni] = tempPonteiros[j];
        ApTemp->UU.U0.ni++;
    }
    ApTemp->UU.U0.pi[ApTemp->UU.U0.ni] = tempPonteiros[MM+1];

    *ApRetorno = ApTemp;
}

void InsereBStar(TipoRegistro Reg, TipoApontador* Ap) {
    short Cresceu;
    TipoRegistro RegRetorno;
    TipoApontador ApRetorno, ApTemp;

    if (*Ap == NULL) {
        *Ap = (TipoApontador) malloc(sizeof(TipoPagina));
        if (*Ap == NULL) { return; }
        (*Ap)->Pt = Externa;
        (*Ap)->UU.U1.ne = 1;
        (*Ap)->UU.U1.re[0] = Reg;
        return;
    }

    InsBStar(Reg, *Ap, &Cresceu, &RegRetorno, &ApRetorno);

    if (Cresceu) {
        ApTemp = (TipoApontador) malloc(sizeof(TipoPagina));
        if (ApTemp == NULL) { return; }
        ApTemp->Pt = Interna;
        ApTemp->UU.U0.ni = 1;
        ApTemp->UU.U0.ri[0] = RegRetorno.Chave;
        ApTemp->UU.U0.pi[0] = *Ap;
        ApTemp->UU.U0.pi[1] = ApRetorno;
        *Ap = ApTemp;
    }
}

void LiberaBStar(TipoApontador Ap) {
    if (Ap == NULL) return;
    if (Ap->Pt == Interna) {
        for (int i = 0; i <= Ap->UU.U0.ni; i++) {
            LiberaBStar(Ap->UU.U0.pi[i]);
        }
    }
    free(Ap);
}

// Imprime os contadores de operações da árvore B*
void print_counters_bstar() {
    printf("Pré-processamento:\n");
    printf("    Transferencias: %d\n", TRANSF_EXT_INT_BSTAR_PRE);
    printf("    Comparacoes: %d\n", COMP_CHAVES_BSTAR_PRE);
    printf("Pós-processamento:\n");
    printf("    Transferencias: %d\n", TRANSF_EXT_INT_BSTAR_POS);
    printf("    Comparacoes: %d\n", COMP_CHAVES_BSTAR_POS);
    printf("Total:\n");
    printf("    Transferencias: %d\n", TRANSF_EXT_INT_BSTAR_PRE + TRANSF_EXT_INT_BSTAR_POS);
    printf("    Comparacoes: %d\n", COMP_CHAVES_BSTAR_PRE + COMP_CHAVES_BSTAR_POS);
}

// *** FUNÇÃO PARA CONSTRUIR A ÁRVORE A PARTIR DO ARQUIVO BINÁRIO ***
void ConstroiArvoreDoArquivo(TipoApontador* Arvore, const char* nome_arquivo, int quantidade) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel abrir o arquivo '%s'. Verifique se ele existe.\n", nome_arquivo);
        return;
    }

    TipoRegistro reg;
    int i = 0;
    while (fread(&reg, sizeof(TipoRegistro), 1, arquivo) == 1 && i < quantidade) {
        InsereBStar(reg, Arvore);
        i++;
    }

    fclose(arquivo);
    printf("Arvore B* construida com sucesso a partir do arquivo '%s'.\n", nome_arquivo);
}

