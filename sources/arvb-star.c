#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

// Definições de constantes e tipos
#define M 2
#define MM (2*M)
#define M2 2
#define MM2 (2*M2)

// Contadores para pré-processamento (construção da árvore B*)
int TRANSF_EXT_INT_BSTAR_PRE = 0;
int COMP_CHAVES_BSTAR_PRE = 0;

// Contadores para pós-processamento (busca)
int TRANSF_EXT_INT_BSTAR_POS = 0;
int COMP_CHAVES_BSTAR_POS = 0;

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
// ================================

// Inicializa a árvore B* (define ponteiro raiz como NULL)
void InicializaBStar(TipoApontador* Arvore) {
    *Arvore = NULL;
}

// Função de pesquisa
void PesquisaBStar(TipoRegistro* x, TipoApontador Ap) {
    int i;
    if (Ap == NULL) {
        printf("Chave %ld nao encontrada\n", x->Chave);
        return;
    }

    if (Ap->Pt == Interna) {
        i = 0;
        while (i < Ap->UU.U0.ni && x->Chave >= Ap->UU.U0.ri[i]) {
            COMP_CHAVES_BSTAR_POS++;
            i++;
        }
        PesquisaBStar(x, Ap->UU.U0.pi[i]);
        return;
    }

    // Se a página for externa, procura o registro
    for (i = 0; i < Ap->UU.U1.ne; i++) {
        COMP_CHAVES_BSTAR_POS++;
        if (x->Chave == Ap->UU.U1.re[i].Chave) {
            *x = Ap->UU.U1.re[i];
            printf("Chave %ld encontrada\n", x->Chave);
            return;
        }
    }
    printf("Chave %ld nao encontrada\n", x->Chave);
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

void ImprimeArvoreFormatada(TipoApontador Arvore) {
    printf("\n===========================\n");
    printf("    REPRESENTACAO DA ARVORE B*\n");
    printf("===========================\n");
    ImprimeBStar(Arvore, 0);
    printf("===========================\n\n");
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

// ** INÍCIO DAS FUNÇÕES DE REMOÇÃO **

// CORRIGIDO: Função auxiliar para remover uma chave e um ponteiro de uma página interna
// A correção garante que os ponteiros sejam deslocados corretamente.
void RemoveDaPaginaInterna(TipoPagina* Pag, int pos) {
    // Desloca as chaves para a esquerda
    for (int i = pos; i < Pag->UU.U0.ni - 1; i++) {
        Pag->UU.U0.ri[i] = Pag->UU.U0.ri[i + 1];
    }
    // Desloca os ponteiros para a esquerda.
    // O ponteiro à direita da chave removida (pi[pos+1]) e todos os seguintes
    // devem ser movidos uma posição para a esquerda.
    for (int i = pos + 1; i <= Pag->UU.U0.ni; i++) {
        Pag->UU.U0.pi[i - 1] = Pag->UU.U0.pi[i];
    }
    Pag->UU.U0.ni--;
}

// Função auxiliar para remover um registro de uma página externa
void RemoveDaPaginaExterna(TipoPagina* Pag, int pos) {
    for (int i = pos; i < Pag->UU.U1.ne - 1; i++) {
        Pag->UU.U1.re[i] = Pag->UU.U1.re[i + 1];
    }
    Pag->UU.U1.ne--;
}

// Função recursiva para remoção
void Rem(TipoRegistro Reg, TipoApontador Ap, short* Diminuiu) {
    int i;
    *Diminuiu = false;

    if (Ap == NULL) return; // Não encontrou a chave

    if (Ap->Pt == Externa) {
        // Encontra o registro na página folha
        for (i = 0; i < Ap->UU.U1.ne; i++) {
            COMP_CHAVES_BSTAR_POS++;
            if (Ap->UU.U1.re[i].Chave == Reg.Chave) {
                // Remove o registro
                RemoveDaPaginaExterna(Ap, i);
                
                // Verifica underflow com a regra B-tree (quantidade < M2)
                if (Ap->UU.U1.ne < M2) {
                    *Diminuiu = true;
                }
                return;
            }
        }
        return; // Chave não encontrada na folha
    }

    // Se for uma página interna, desce na árvore
    i = 0;
    while (i < Ap->UU.U0.ni && Reg.Chave >= Ap->UU.U0.ri[i]) {
        COMP_CHAVES_BSTAR_POS++;
        i++;
    }

    // Chamada recursiva para o filho
    Rem(Reg, Ap->UU.U0.pi[i], Diminuiu);

    // Lógica de rebalanceamento após o retorno da recursão
    if (*Diminuiu) {
        TipoApontador Filho = Ap->UU.U0.pi[i];
        TipoApontador Esquerdo = NULL;
        TipoApontador Direito = NULL;
        int pos_separador_filho = i;

        // Tenta encontrar um irmão à esquerda ou direita
        if (i > 0) Esquerdo = Ap->UU.U0.pi[i - 1];
        if (i < Ap->UU.U0.ni) Direito = Ap->UU.U0.pi[i + 1];

        // Caso 1: Redistribuição do irmão direito (se tiver mais que o mínimo)
        if (Direito != NULL && Filho->Pt == Direito->Pt) {
            if ((Direito->Pt == Externa && Direito->UU.U1.ne > M2) ||
                (Direito->Pt == Interna && Direito->UU.U0.ni > M)) {
                
                if (Filho->Pt == Externa) {
                    InsereNaPaginaExterna(Filho, Direito->UU.U1.re[0]);
                    RemoveDaPaginaExterna(Direito, 0);
                    Ap->UU.U0.ri[pos_separador_filho] = Direito->UU.U1.re[0].Chave;
                } else {
                    // Mover a chave do pai para o filho
                    InsereNaPaginaInterna(Filho, Ap->UU.U0.ri[pos_separador_filho], Direito->UU.U0.pi[0]);
                    // Mover a primeira chave do irmão para o pai
                    Ap->UU.U0.ri[pos_separador_filho] = Direito->UU.U0.ri[0];
                    // Remover a chave e o ponteiro movido do irmão
                    RemoveDaPaginaInterna(Direito, 0);
                }
                *Diminuiu = false;
                return;
            }
        }

        // Caso 2: Redistribuição do irmão esquerdo (se tiver mais que o mínimo)
        if (Esquerdo != NULL && Filho->Pt == Esquerdo->Pt) {
            if ((Esquerdo->Pt == Externa && Esquerdo->UU.U1.ne > M2) ||
                (Esquerdo->Pt == Interna && Esquerdo->UU.U0.ni > M)) {
                
                if (Filho->Pt == Externa) {
                    InsereNaPaginaExterna(Filho, Esquerdo->UU.U1.re[Esquerdo->UU.U1.ne - 1]);
                    Esquerdo->UU.U1.ne--;
                    Ap->UU.U0.ri[pos_separador_filho - 1] = Filho->UU.U1.re[0].Chave;
                } else {
                    TipoChave chave_separadora_pai = Ap->UU.U0.ri[pos_separador_filho - 1];
                    TipoApontador ponteiro_ultimo = Esquerdo->UU.U0.pi[Esquerdo->UU.U0.ni];
                    InsereNaPaginaInterna(Filho, chave_separadora_pai, ponteiro_ultimo);
                    Ap->UU.U0.ri[pos_separador_filho - 1] = Esquerdo->UU.U0.ri[Esquerdo->UU.U0.ni - 1];
                    Esquerdo->UU.U0.ni--;
                }
                *Diminuiu = false;
                return;
            }
        }
        
        // Caso 3: Concatenação
        TipoApontador a_mesclar_com = NULL;
        int pos_chave_a_remover = -1;

        if (Esquerdo != NULL) {
            a_mesclar_com = Esquerdo;
            pos_chave_a_remover = pos_separador_filho - 1;
        } else if (Direito != NULL) {
            a_mesclar_com = Direito;
            pos_chave_a_remover = pos_separador_filho;
        }
        
        if (a_mesclar_com != NULL) {
            if (Filho->Pt == Externa) { // Mescla de folhas
                for (int k = 0; k < Filho->UU.U1.ne; k++) {
                    a_mesclar_com->UU.U1.re[a_mesclar_com->UU.U1.ne] = Filho->UU.U1.re[k];
                    a_mesclar_com->UU.U1.ne++;
                }
            } else { // Mescla de nós internos
                TipoChave chave_pai = Ap->UU.U0.ri[pos_chave_a_remover];
                a_mesclar_com->UU.U0.ri[a_mesclar_com->UU.U0.ni] = chave_pai;
                a_mesclar_com->UU.U0.pi[a_mesclar_com->UU.U0.ni] = Filho->UU.U0.pi[0];
                a_mesclar_com->UU.U0.ni++;

                for (int k = 0; k < Filho->UU.U0.ni; k++) {
                    a_mesclar_com->UU.U0.ri[a_mesclar_com->UU.U0.ni] = Filho->UU.U0.ri[k];
                    a_mesclar_com->UU.U0.pi[a_mesclar_com->UU.U0.ni + 1] = Filho->UU.U0.pi[k + 1];
                    a_mesclar_com->UU.U0.ni++;
                }
            }
            free(Filho);
            RemoveDaPaginaInterna(Ap, pos_chave_a_remover);
            
            // Verifica underflow no nó pai com a nova regra (ni < M)
            if (Ap->UU.U0.ni < M) {
                *Diminuiu = true;
            } else {
                *Diminuiu = false;
            }
        }
    }
}

void RemoveBStar(TipoRegistro Reg, TipoApontador* Ap) {
    short Diminuiu;
    Rem(Reg, *Ap, &Diminuiu);

    // Se a raiz se tornou vazia (apenas um ponteiro), a altura diminuiu
    if (Diminuiu && (*Ap)->UU.U0.ni == 0 && (*Ap)->Pt == Interna) {
        TipoApontador nova_raiz = (*Ap)->UU.U0.pi[0];
        free(*Ap);
        *Ap = nova_raiz;
    }
    // Se a raiz se tornou um nó folha vazio, limpa a árvore
    if (*Ap != NULL && (*Ap)->Pt == Externa && (*Ap)->UU.U1.ne == 0) {
        free(*Ap);
        *Ap = NULL;
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

// *** FUNÇÃO DE CRIAÇÃO DE ARQUIVO BINÁRIO (para teste) ***
void CriaArquivoBinarioDeTeste(const char* nome_arquivo) {
    FILE* arquivo = fopen(nome_arquivo, "wb");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel criar o arquivo binario '%s'.\n", nome_arquivo);
        return;
    }

    TipoRegistro reg;
    long chaves_teste[] = {10, 50, 30, 20, 60, 40, 70, 80, 5, 15, 25, 35, 45, 55, 65, 75, 85, 90, 100, 1, 95};
    int num_chaves = sizeof(chaves_teste) / sizeof(long);

    printf("Criando arquivo binario de teste '%s' com %d chaves...\n", nome_arquivo, num_chaves);
    for (int i = 0; i < num_chaves; i++) {
        reg.Chave = chaves_teste[i];
        fwrite(&reg, sizeof(TipoRegistro), 1, arquivo); 
    }

    fclose(arquivo);
    printf("Arquivo binario criado com sucesso.\n");
}

// *** FUNÇÃO PARA CONSTRUIR A ÁRVORE A PARTIR DO ARQUIVO BINÁRIO ***
void ConstroiArvoreDoArquivo(TipoApontador* Arvore, const char* nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel abrir o arquivo '%s'. Verifique se ele existe.\n", nome_arquivo);
        return;
    }

    TipoRegistro reg;
    while (fread(&reg, sizeof(TipoRegistro), 1, arquivo) == 1) {
        InsereBStar(reg, Arvore);
    }

    fclose(arquivo);
    printf("Arvore B* construida com sucesso a partir do arquivo '%s'.\n", nome_arquivo);
}

// Função principal para teste
int main() {
    TipoApontador arvore = NULL;
    char nome_arquivo[] = "dados.bin";

    // Cria o arquivo binário de teste
    CriaArquivoBinarioDeTeste(nome_arquivo);
    
    // Constrói a árvore a partir do arquivo
    InicializaBStar(&arvore);
    ConstroiArvoreDoArquivo(&arvore, nome_arquivo);

    // Imprime a árvore para verificar a estrutura.
    printf("\n--- ARVORE APOS INSERCAO ---\n");
    ImprimeArvoreFormatada(arvore);

    // Exemplo de remoção de chaves.
    TipoRegistro reg_remover;
    long chaves_para_remover[] = {25, 30, 60, 50, 40, 10}; // Chaves para testar a remoção
    printf("\n--- REMOCAO DE REGISTROS ---\n");
    for(int i = 0; i < sizeof(chaves_para_remover) / sizeof(long); i++) {
        reg_remover.Chave = chaves_para_remover[i];
        printf("Removendo chave: %ld\n", reg_remover.Chave);
        RemoveBStar(reg_remover, &arvore);
        ImprimeArvoreFormatada(arvore);
    }

    // Exemplo de pesquisa após remoções
    TipoRegistro reg_busca;
    long chave_para_buscar;
    printf("\n--- TESTE DE BUSCA APOS REMOCAO ---\n");
    printf("Digite uma chave para buscar na arvore (ou Ctrl+D/Ctrl+Z para sair):\n");
    
    while (scanf("%ld", &chave_para_buscar) == 1) {
        reg_busca.Chave = chave_para_buscar;
        printf("\nProcurando chave %ld...\n", reg_busca.Chave);
        PesquisaBStar(&reg_busca, arvore);
        printf("\nDigite uma nova chave para buscar: ");
    }

    printf("\n");
    print_counters_bstar();

    LiberaBStar(arvore);

    return 0;
}