#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "include/reg.h"

#define MIN_ARGS 3
#define MAX_ARGS 4

#define N_REGS 1
#define ORDER 2
#define OUT_FILE 3


typedef struct {
    int n_regs;
    char *ord;
    char *out;

} command_params_t;

void help(void) {
    printf("Esse programa foi criado com o intuito que você possa criar registros aleatórios.\n");
    printf("Quantos quiser, na ordem de chave que quiser\n A sintaxe de uso é a seguinte:\n");
    printf("./create_bin <Numero de registros desejado> <Ordem das chaves> <Arquivo de saída>\n");
    printf("Bom uso!");

}


bool verify_num_of_registers(char *n_param) {

    int num;

    char c;
    return (sscanf(n_param, "%d%c", &num, &c) == 1);

}

bool verify_order(char *ord) {

   return (strcmp(ord, "asc") == 0 ) ? true : (strcmp(ord, "desc") == 0) ? true : (strcmp(ord, "al") == 0) ? true : false;
}
bool get_params(command_params_t *params, int argc, char **argv){
    char *num_of_regs = argv[N_REGS];
    if (!verify_num_of_registers(num_of_regs)){
        printf("O primeiro argumento precisa ser um número inteiro!\n");
        return false;
    }

    params->n_regs = atoi(num_of_regs);

    char *ord = argv[ORDER];

    if (!verify_order(ord)) {
        printf("A ordem deve ser Ascendente (asc), Descendente (desc) ou Aleatorio (al)\n");
        return false;
    }

    params->ord = ord;

    if (argc == MAX_ARGS)
        params->out = argv[OUT_FILE];
    else if (argc == MIN_ARGS)
        params->out = "regs.bin";

    return true;
}

void gen_rand_string(char *out_buff, int n_chars) {

    for (int i = 0; i < n_chars-1; i++){
        int random;
        do {
            random = rand() % 124;
        }
        while (!(random >= 'A' && random <= 'Z') &&  !(random >= 'a' && random <= 'z'));

        out_buff[i] = (char)random;
    }
    out_buff[n_chars-1] = '\0';
}
int main(int argc, char **argv){

    if (argc < MIN_ARGS || argc > MAX_ARGS){
        printf("Foram passados %d argumentos, você precisa de pelo menos 2 e no máximo 3\n\n", argc -1 );
        help();
        return 0;
    }

    command_params_t params;
    if (!get_params(&params, argc, argv))
        return 0;

    srand(time(NULL));

    FILE *fp = fopen(params.out, "w+b");

    int *keys_used = NULL;

    if (strcmp(params.ord, "al") == 0)
        keys_used = (int*)calloc(params.n_regs, sizeof(int));

    for (int i = 0; i < params.n_regs; i++) {
        reg_t new_reg;
        if (strcmp(params.ord, "asc") == 0)
            new_reg.chave = i;
        else if (strcmp(params.ord, "desc") == 0)
            new_reg.chave = params.n_regs -1 - i;
        else{
            int k_gen;
            do {
                k_gen = rand() % params.n_regs;
            }while(keys_used[k_gen] == 1);
            keys_used[k_gen] = 1;
            new_reg.chave = k_gen;
        }

        new_reg.dado1 = rand();
        gen_rand_string(new_reg.dado2, 1000);
        gen_rand_string(new_reg.dado3, 5000);

        if (fwrite(&new_reg, sizeof(reg_t), 1, fp) != 1){
            printf("Uncussefully write of register number %d\n Shutting down...\n", i+1);
            fclose(fp);
            return -1;

        }

    }

}
