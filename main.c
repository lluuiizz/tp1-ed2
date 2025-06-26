#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "include/asi.h"
#include "include/abin.h"
#include "include/arvb.h"
#include "include/arvb-star.h"

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Uso: ./main <método> <quantidade> <situação> <chave> [-P]\n");
        return 1;
    }

    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    int chave = atoi(argv[4]);
    bool exibirChaves = (argc > 6 && (strcmp(argv[5], "-P") == 0) ? true : false);

    char nomeArquivo[64] = {0};
    if (situacao == 1) {
        printf("estou em situacao 1");
        strcpy(nomeArquivo, "regs_bin/asc.bin");
        nomeArquivo[17] = '\0';

    }
    else if (situacao == 2){
        strcpy(nomeArquivo, "regs_bin/desc.bin");
        nomeArquivo[18] = '\0';
    }
    else{
        strcpy(nomeArquivo, "regs_bin/al.bin");
        nomeArquivo[16] = '\0';

    }

    switch (metodo) {
        case 1:
            // Pesquisa sequencial indexada
            {
                if (situacao != 1 && situacao != 2) {
                    printf("Situação %d não é suportada para pesquisa sequencial indexada.\n", situacao);
                    return 1;
                }
                FILE *fp = fopen(nomeArquivo, "rb");
                if (!fp) {
                    printf("Erro ao abrir arquivo %s\n", nomeArquivo);
                    return 1;
                }

                index_table_t table;
                alloc_index_table(&table, quantidade);
                create_index_table(fp, &table);

                reg_t x;
                x.chave = chave;
                fseek(fp, 0, SEEK_SET);
                bool found;
                if (situacao == 1)
                    found = search(fp, "asc", table, &x);
                else
                    found = search(fp, "desc", table, &x);

                if (found){
                    printf("Registro encontrado: chave = %d\n", x.chave);
                    printf("Registro :: chave = %d :: Dado1 = %ld\n\n",x.chave, x.dado1);

                }
                else
                    printf("Registro não encontrado\n");

                if (exibirChaves) {
                    for (int i = 6; i < argc; i++) {
                        reg_t x;
                        x.chave = atoi(argv[i]);
                        if (situacao == 1)
                            found = search(fp, "asc", table, &x);
                        else
                            found = search(fp, "desc", table, &x);

                        if (found){
                            printf("Registro encontrado: chave = %d\n", x.chave);
                            printf("Registro :: chave = %d :: Dado1 = %ld\n\n",x.chave, x.dado1);

                        }
                        else
                            printf("Registro não encontrado, chave = %d\n", x.chave);

                    }
                }

                print_counters_asi();
                fclose(fp);
                free(table.table);
            }
            break;
        case 2:
            // Árvore binária de pesquisa
            {
                FILE *fp = fopen(nomeArquivo, "rb");
                if (!fp) {
                    printf("Erro ao abrir arquivo %s\n", nomeArquivo);
                    return 1;
                }

                create_binary_tree(fp, quantidade);
                fclose(fp);

                FILE *btree = fopen("binary_tree.bin", "rb");
                if (btree == NULL) {
                    printf("Erro ao abrir arquivo binary_tree.bin\n");
                    return 1;
                }

                reg_t x;
                x.chave = chave;
                bool found = binary_search(btree, &x);

                if (found){
                    printf("Registro encontrado: chave = %d\n", x.chave);
                    printf("Registro :: chave = %d :: Dado1 = %ld\n\n",x.chave, x.dado1);

                }
                else
                    printf("Registro não encontrado\n");
                if (exibirChaves) {
                    for (int i = 6; i < argc; i++) {
                        fseek(btree, 0, SEEK_SET);
                        reg_t x;
                        x.chave = atoi(argv[i]);
                        found = binary_search(btree, &x);

                        if (found){
                            printf("Registro encontrado: chave = %d\n", x.chave);
                            printf("Registro :: chave = %d :: Dado1 = %ld\n\n",x.chave, x.dado1);

                        }
                        else
                            printf("Registro não encontrado, chave = %d\n", x.chave);
                    }
                }
                print_counters_abin();
                fclose(btree);
            }
            break;
        case 3:
            // Árvore B
            {
                tipo_apontador arvore = construir_arvore_b(nomeArquivo, quantidade);

                if (arvore == NULL)
                    return -1;
                tipo_registro x;
                x.chave = chave;
                bool found;
                if (arvore)
                    found = pesquisa_arvore_b(&x, arvore);

                if (found ){
                    printf("Registro encontrado: chave = %d\n", x.chave);
                    printf("Registro :: chave = %d :: Dado1 = %ld\n\n",x.chave, x.dado1);
                }
                else
                    printf("Registro não encontrado\n");

                if (exibirChaves) {
                    for (int i = 6; i < argc; i++) {
                        tipo_registro x;
                        x.chave = atoi(argv[i]);
                        found = pesquisa_arvore_b(&x, arvore);

                        if (found){
                            printf("Registro encontrado: chave = %d\n", x.chave);
                            printf("Registro :: chave = %d :: Dado1 = %ld\n\n",x.chave, x.dado1);
                        }
                        else
                            printf("Registro não encontrado: chave = %d\n\n", x.chave);
                    }
                }

                print_counters_arvb();
                if (arvore)
                    libera_arvore_b(arvore);

                arvore = NULL;
                assert(arvore == NULL);
            }
            break;
        case 4:
            printf("Para fazer ainda B-Estrela!\n");


            break;
        default:
            printf("Método inválido! Use 1 a 4.\n");
            return 1;
    }
    return 0;
}
