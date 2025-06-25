#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "include/reg.h"

// Define o número mínimo e máximo de argumentos esperados na linha de comando
#define MIN_ARGS 3
#define MAX_ARGS 4

// Define os índices dos argumentos na linha de comando
#define N_REGS 1
#define ORDER 2
#define OUT_FILE 3

// Estrutura para armazenar os parâmetros do comando
typedef struct {
    int n_regs;     // Número de registros a serem gerados
    char *ord;      // Ordem das chaves ("asc", "desc" ou "al")
    char *out;      // Nome do arquivo de saída
} command_params_t;

// Função que imprime instruções de uso do programa
void help(void) {
    printf("Esse programa foi criado com o intuito que você possa criar registros aleatórios.\n");
    printf("Quantos quiser, na ordem de chave que quiser\n A sintaxe de uso é a seguinte:\n");
    printf("./create_bin <Numero de registros desejado> <Ordem das chaves> <Arquivo de saída>\n");
    printf("Bom uso!");
}

// Verifica se o argumento passado é um número inteiro válido
bool verify_num_of_registers(char *n_param) {
    int num;
    char c;
    return (sscanf(n_param, "%d%c", &num, &c) == 1);
}

// Verifica se a ordem passada é válida ("asc", "desc" ou "al")
bool verify_order(char *ord) {
   return (strcmp(ord, "asc") == 0 ) ? true : (strcmp(ord, "desc") == 0) ? true : (strcmp(ord, "al") == 0) ? true : false;
}

// Lê e valida os parâmetros da linha de comando, preenchendo a struct params
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

    // Define o nome do arquivo de saída, se não for passado usa "regs.bin"
    if (argc == MAX_ARGS)
        params->out = argv[OUT_FILE];
    else if (argc == MIN_ARGS)
        params->out = "regs.bin";

    return true;
}

// Gera uma string aleatória de tamanho n_chars-1 (último caractere é '\0')
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
    // Verifica a quantidade de argumentos
    if (argc < MIN_ARGS || argc > MAX_ARGS){
        printf("Foram passados %d argumentos, você precisa de pelo menos 2 e no máximo 3\n\n", argc -1 );
        help();
        return 0;
    }

    command_params_t params;
    // Lê e valida os parâmetros
    if (!get_params(&params, argc, argv))
        return 0;

    srand(time(NULL)); // Inicializa o gerador de números aleatórios

    FILE *fp = fopen(params.out, "w+b"); // Abre o arquivo de saída para escrita binária

    int *keys_used = NULL;

    // Se a ordem for aleatória, cria um vetor para controlar as chaves já usadas
    if (strcmp(params.ord, "al") == 0)
        keys_used = (int*)calloc(params.n_regs, sizeof(int));

    // Gera os registros conforme o número solicitado
    for (int i = 0; i < params.n_regs; i++) {
        reg_t new_reg;
        // Define a chave conforme a ordem escolhida
        if (strcmp(params.ord, "asc") == 0)
            new_reg.chave = i;
        else if (strcmp(params.ord, "desc") == 0)
            new_reg.chave = params.n_regs -1 - i;
        else{
            // Gera uma chave aleatória não repetida
            int k_gen;
            do {
                k_gen = rand() % params.n_regs;
            }while(keys_used[k_gen] == 1);
            keys_used[k_gen] = 1;
            new_reg.chave = k_gen;
        }

        new_reg.dado1 = rand(); // Gera um inteiro aleatório para dado1
        gen_rand_string(new_reg.dado2, 1000); // Gera string aleatória para dado2
        gen_rand_string(new_reg.dado3, 5000); // Gera string aleatória para dado3

        // Escreve o registro no arquivo binário
        if (fwrite(&new_reg, sizeof(reg_t), 1, fp) != 1){
            printf("Uncussefully write of register number %d\n Shutting down...\n", i+1);
            fclose(fp);
            return -1;
        }
    }
}