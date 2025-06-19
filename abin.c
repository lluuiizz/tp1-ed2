#include "include/abin.h"

void find_dad(FILE *binary_arq, int child_key, int child_pos) {
    reg_abin_t act_reg;
    fread(&act_reg, sizeof(reg_abin_t), 1, binary_arq);

    int act_key = act_reg.reg.chave;

    // Go to left node
    if (child_key < act_key) {
        if (act_reg.left_node == -1){
            act_reg.left_node = child_pos;

            long where_am_i = ftell(binary_arq);
            fseek(binary_arq, where_am_i - sizeof(reg_abin_t), SEEK_SET);
            fwrite(&act_reg, sizeof(reg_abin_t), 1, binary_arq);
            return;
        }
        else {
            long desloc = act_reg.left_node * sizeof(reg_abin_t);
            fseek(binary_arq, desloc, SEEK_SET);
            find_dad(binary_arq, child_key, child_pos);
        }
    }
    // Go to right node
    else if (child_key > act_key){
        if (act_reg.right_node == -1){
            act_reg.right_node = child_pos;

            long where_am_i = ftell(binary_arq);
            fseek(binary_arq, where_am_i - sizeof(reg_abin_t), SEEK_SET);
            fwrite(&act_reg, sizeof(reg_abin_t), 1, binary_arq);
            return;
        }
        else {
            long desloc = act_reg.right_node * sizeof(reg_abin_t);
            fseek(binary_arq, desloc, SEEK_SET);
            find_dad(binary_arq, child_key, child_pos);
        }
    }
    else
        return;
}

bool add_node(FILE *binary_arq, reg_t node_reg, int top){
    // Alocate the new node
    reg_abin_t new_node;
    new_node.left_node = -1;
    new_node.right_node = -1;
    new_node.reg = node_reg;
    fseek(binary_arq, 0, SEEK_END);
    fwrite(&new_node, sizeof(reg_abin_t), 1, binary_arq);

    // If it was the first node, so it returns
    if (top == 0)
        return true;

    // Finds the father of the new node
    int node_key = node_reg.chave;
    fseek(binary_arq, 0, SEEK_SET);

    find_dad(binary_arq, node_key, top);

    return true;

}
bool create_binary_tree(FILE *source, int qnt_regs) {

    FILE *dest;

    if (source == NULL || (dest = fopen("binary_tree.bin", "w+b")) == NULL)
        return false;

    int top = 0;

    reg_t reg;
    for (int i = 0; i <= qnt_regs; i++){
        fread(&reg, sizeof(reg_t), 1, source);
        add_node(dest, reg, top);
        top++;
    }

    return true;

}
