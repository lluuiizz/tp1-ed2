#include "include/abin.h"
#include <string.h>

// Contadores para pré-processamento (criação da árvore)
int TRANSF_EXT_INT_ABIN_PRE = 0;
int COMP_CHAVES_ABIN_PRE = 0;

// Contadores para pós-processamento (pesquisa)
int TRANSF_EXT_INT_ABIN_POS = 0;
int COMP_CHAVES_ABIN_POS = 0;




void find_dad(FILE *binary_arq, int child_key, int child_pos) {
    reg_abin_t act_reg;
    long current_pos = ftell(binary_arq); // Posição inicial (normalmente 0 para a raiz)

    while (1) {
        fseek(binary_arq, current_pos, SEEK_SET); // Vai para o nó atual
        fread(&act_reg, sizeof(reg_abin_t), 1, binary_arq); // Lê o registro
        TRANSF_EXT_INT_ABIN_PRE++; // Contador de transferências

        int act_key = act_reg.reg.chave;

        // Se a chave do filho é menor, tenta ir para a esquerda
        if (child_key < act_key) {
            COMP_CHAVES_ABIN_PRE++; // Contador de comparações
            if (act_reg.left_node == -1) { // Encontrou o lugar para inserir
                act_reg.left_node = child_pos; // Atualiza o ponteiro esquerdo
                fseek(binary_arq, current_pos, SEEK_SET); // Volta para o nó atual
                fwrite(&act_reg, sizeof(reg_abin_t), 1, binary_arq); // Escreve a atualização
                return;
            } else {
                current_pos = act_reg.left_node * sizeof(reg_abin_t); // Vai para o filho esquerdo
            }
        }
        // Se a chave do filho é maior, tenta ir para a direita
        else if (child_key > act_key) {
            COMP_CHAVES_ABIN_PRE++; // Contador de comparações
            if (act_reg.right_node == -1) { // Encontrou o lugar para inserir
                act_reg.right_node = child_pos; // Atualiza o ponteiro direito
                fseek(binary_arq, current_pos, SEEK_SET); // Volta para o nó atual
                fwrite(&act_reg, sizeof(reg_abin_t), 1, binary_arq); // Escreve a atualização
                return;
            } else {
                current_pos = act_reg.right_node * sizeof(reg_abin_t); // Vai para o filho direito
            }
        }
        // Se a chave já existe, não faz nada (evita duplicatas)
        else {
            COMP_CHAVES_ABIN_PRE++; // Contador de comparações
            return;
        }
    }
}
// Função recursiva para encontrar o "pai" de um novo nó na árvore binária
/*
void find_dad(FILE *binary_arq, int child_key, int child_pos) {
    reg_abin_t act_reg;
    fread(&act_reg, sizeof(reg_abin_t), 1, binary_arq); // Lê o nó atual do arquivo
    TRANSF_EXT_INT_ABIN_PRE++; // Conta transferência de item

    int act_key = act_reg.reg.chave;

    // Se a chave do filho é menor, vai para o nó à esquerda
    if (child_key < act_key) {
        COMP_CHAVES_ABIN_PRE++; // Conta comparação de chave
        if (act_reg.left_node == -1){ // Se não há filho à esquerda, insere aqui
            act_reg.left_node = child_pos;

            long where_am_i = ftell(binary_arq); // Salva posição atual
            fseek(binary_arq, where_am_i - sizeof(reg_abin_t), SEEK_SET); // Volta para o início do nó
            fwrite(&act_reg, sizeof(reg_abin_t), 1, binary_arq); // Atualiza o nó com o novo filho
            return;
        }
        else {
            // Vai recursivamente para o filho à esquerda
            long desloc = act_reg.left_node * sizeof(reg_abin_t);
            fseek(binary_arq, desloc, SEEK_SET);
            find_dad(binary_arq, child_key, child_pos);
        }
    }
    // Se a chave do filho é maior, vai para o nó à direita
    else if (child_key > act_key){
        COMP_CHAVES_ABIN_PRE++; // Conta comparação de chave
        if (act_reg.right_node == -1){ // Se não há filho à direita, insere aqui
            act_reg.right_node = child_pos;

            long where_am_i = ftell(binary_arq);
            fseek(binary_arq, where_am_i - sizeof(reg_abin_t), SEEK_SET);
            fwrite(&act_reg, sizeof(reg_abin_t), 1, binary_arq);
            return;
        }
        else {
            // Vai recursivamente para o filho à direita
            long desloc = act_reg.right_node * sizeof(reg_abin_t);
            fseek(binary_arq, desloc, SEEK_SET);
            find_dad(binary_arq, child_key, child_pos);
        }
    }
    else {
        COMP_CHAVES_ABIN_PRE++; // Conta comparação de chave (igual)
        return; // Se for igual, não faz nada (não insere duplicados)
    }
}
*/

// Adiciona um novo nó à árvore binária no arquivo
bool add_node(FILE *binary_arq, reg_t node_reg, int top){
    // Cria o novo nó com ponteiros para filhos nulos
    reg_abin_t new_node;
    new_node.left_node = -1;
    new_node.right_node = -1;
    new_node.reg = node_reg;
    fseek(binary_arq, 0, SEEK_END); // Vai para o final do arquivo
    fwrite(&new_node, sizeof(reg_abin_t), 1, binary_arq); // Escreve o novo nó
    TRANSF_EXT_INT_ABIN_PRE++; // Conta transferência de item

    // Se for o primeiro nó (raiz), não precisa procurar pai
    if (top == 0)
        return true;

    // Procura o pai do novo nó e atualiza o ponteiro correspondente
    int node_key = node_reg.chave;
    fseek(binary_arq, 0, SEEK_SET); // Volta para o início do arquivo
    find_dad(binary_arq, node_key, top);

    return true;
}

// Cria uma árvore binária de busca a partir de um arquivo de registros
bool create_binary_tree(FILE *source, int qnt_regs) {

    FILE *dest;

    // Verifica se o arquivo de origem existe e abre o arquivo de destino
    if (source == NULL || (dest = fopen("binary_tree.bin", "w+b")) == NULL)
        return false;

    int top = 0;

    reg_t reg;
    // Lê cada registro do arquivo de origem e adiciona na árvore binária
    for (int i = 0; i < qnt_regs; i++){
        fread(&reg, sizeof(reg_t), 1, source);
        TRANSF_EXT_INT_ABIN_PRE++; // Conta transferência de item
        add_node(dest, reg, top);
        top++;
    }

    fclose(dest); // Fecha o arquivo da árvore
    return true;
}

// Busca binária recursiva em arquivo de árvore binária
bool binary_search(FILE *source, reg_t *x) {
    if (source == NULL) return false;

    int search_key = x->chave;
    reg_abin_t reg_node;

    rewind(source); // Garante que começamos do início

    while (1) {
        if (fread(&reg_node, sizeof(reg_abin_t), 1, source) != 1) {
            return false; // Falha na leitura
        }
        TRANSF_EXT_INT_ABIN_POS++;

        if (search_key == reg_node.reg.chave) {
            COMP_CHAVES_ABIN_POS++;
            *x = reg_node.reg;
            return true;
        }
        else if (search_key < reg_node.reg.chave) {
            COMP_CHAVES_ABIN_POS++;
            if (reg_node.left_node == -1) return false;
            fseek(source, reg_node.left_node * sizeof(reg_abin_t), SEEK_SET);
        }
        else {
            COMP_CHAVES_ABIN_POS++;
            if (reg_node.right_node == -1) return false;
            fseek(source, reg_node.right_node * sizeof(reg_abin_t), SEEK_SET);
        }
    }
}

/*
bool binary_search(FILE *source, reg_t *x){
    if (source == NULL)
        return false;

    static bool returned_value = false; // Valor de retorno estático para recursão

    int search_key = x->chave;

    reg_abin_t reg_node;
    fread(&reg_node, sizeof(reg_abin_t), 1, source); // Lê o nó atual
    TRANSF_EXT_INT_ABIN_POS++; // Conta transferência de item

    // Se encontrou a chave, copia o registro e retorna true
    if  (search_key == reg_node.reg.chave){
        COMP_CHAVES_ABIN_POS++; // Conta comparação de chave
        *x = reg_node.reg;
        returned_value = true;
        return returned_value;
    }
    // Se a chave buscada é menor, busca à esquerda
    else if (search_key < reg_node.reg.chave){
        COMP_CHAVES_ABIN_POS++; // Conta comparação de chave
        if (reg_node.left_node == -1)
            return false; // Não encontrou
        long desloc = reg_node.left_node * sizeof(reg_abin_t);
        fseek(source, desloc, SEEK_SET);
        return binary_search(source, x);
    }
    // Se a chave buscada é maior, busca à direita
    else {
        COMP_CHAVES_ABIN_POS++; // Conta comparação de chave
        if (reg_node.right_node == -1)
            return false; // Não encontrou
        long desloc = reg_node.right_node * sizeof(reg_abin_t);
        fseek(source, desloc, SEEK_SET);
        return binary_search(source, x);
    }


    return returned_value;
}

*/

void print_counters_abin() { // Função para exibir os contadores de transferências e comparações
    printf("Pré-processamento:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_ABIN_PRE);
    printf("  Comparações: %d\n", COMP_CHAVES_ABIN_PRE);
    printf("Pós-processamento:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_ABIN_POS);
    printf("  Comparações: %d\n", COMP_CHAVES_ABIN_POS);
    printf("Total:\n");
    printf("  Transferências: %d\n", TRANSF_EXT_INT_ABIN_PRE + TRANSF_EXT_INT_ABIN_POS);
    printf("  Comparações: %d\n", COMP_CHAVES_ABIN_PRE + COMP_CHAVES_ABIN_POS);
}
