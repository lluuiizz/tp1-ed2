#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "include/reg.h"


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
    char *num_of_regs = argv[1];
    if (!verify_num_of_registers(num_of_regs)){
        printf("O primeiro argumento precisa ser um número inteiro!\n");
        return false;
    }

    params->n_regs = atoi(num_of_regs);

    char *ord = argv[2];

    if (!verify_order(ord)) {
        printf("A ordem deve ser Ascendente (asc), Descendente (desc) ou Aleatorio (al)\n");
        return false;
    }

    params->ord = ord;

    if (argc == 4)
        params->out = argv[3];
    else if (argc == 3)
        params->out = "regs.bin";

    return true;
}
int main(int argc, char **argv){

    if (argc < 3 || argc > 4){
        printf("Foram passados %d argumentos, você precisa de pelo menos 2 e no máximo 3\n\n", argc -1 );
        help();
        return 0;
    }

    command_params_t params;
    if (!get_params(&params, argc, argv))
        return 0;


}
