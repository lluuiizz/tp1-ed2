#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "include/asi.h"
#include "include/abin.h"
#include "include/arvb.h"
#include "include/arvb-star.h"

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Uso: ./nome-arquivo <método> <quantidade> <situação> <chave> [-P]\n");
        return 1;
    }

    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    int chave = atoi(argv[4]);
    bool exibirChaves = (argc > 6 && (strcmp(argv[5], "-P") == 0) ? true : false);

    char nomeArquivo[64] = "registros.bin";

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

                if (found)
                    printf("Registro encontrado: chave = %d\n", x.chave);
                else
                    printf("Registro não encontrado\n");
                print_counters_asi();

                if (exibirChaves) {
                    for (int i = 6; i < argc; i++) {
                        reg_t x;
                        x.chave = atoi(argv[i]);
                        if (situacao == 1)
                            found = search(fp, "asc", table, &x);
                        else
                            found = search(fp, "desc", table, &x);

                        if (found)
                            printf("Registro encontrado: chave = %d\n", x.chave);
                        else
                            printf("Registro não encontrado\n");
                        print_counters_asi();
                    }
                }
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
                if (!btree) {
                    printf("Erro ao abrir arquivo binary_tree.bin\n");
                    return 1;
                }

                reg_t x;
                x.chave = chave;
                bool found = binary_search(btree, &x);

                if (found)
                    printf("Registro encontrado: chave = %d\n", x.chave);
                else
                    printf("Registro não encontrado\n");
                print_counters_abin();
                if (exibirChaves) {
                    for (int i = 6; i < argc; i++) {
                        reg_t x;
                        x.chave = atoi(argv[i]);
                        found = binary_search(btree, &x);

                        if (found)
                            printf("Registro encontrado: chave = %d\n", x.chave);
                        else
                            printf("Registro não encontrado\n");
                        print_counters_asi();
                    }
                }
                fclose(btree);
            }
            break;
        case 3:
            // Árvore B
            {
                tipo_apontador arvore = construir_arvore_b(nomeArquivo, quantidade);

                if (exibirChaves && arvore) {
                    exibir_reg_arvb(arvore); printf("\n");
                }
                tipo_registro x;
                x.chave = chave;
                bool found;
                if (arvore)
                    found = pesquisa_arvore_b(&x, arvore);

                if (found )
                    printf("Registro encontrado: chave = %d\n", x.chave);
                else
                    printf("Registro não encontrado\n");
                print_counters_arvb();

                if (exibirChaves) {
                    for (int i = 6; i < argc; i++) {
                        tipo_registro x;
                        x.chave = atoi(argv[i]);
                        found = pesquisa_arvore_b(&x, arvore);

                        if (found)
                            printf("Registro encontrado: chave = %d\n", x.chave);
                        else
                            printf("Registro não encontrado\n");
                        print_counters_asi();
                    }
                }
                if (arvore)
                    libera_arvore_b(arvore);
            }
            break;
        case 4:
            // Árvore B*
            {
                tipo_apontador_estrela arvore = NULL;
                inicializa_b_estrela(&arvore);

                FILE *fp = fopen(nomeArquivo, "rb");
                if (!fp) {
                    printf("Erro ao abrir arquivo %s\n", nomeArquivo);
                    return 1;
                }

                reg_t temp;
                for (int i = 0; i < quantidade; i++) {
                    fread(&temp, sizeof(reg_t), 1, fp);
                    tipo_registro_estrela reg;
                    reg.chave = temp.chave;
                    // copie outros campos se necessário
                    insere_b_estrela(reg, &arvore);
                }
                fclose(fp);



                tipo_registro_estrela x;
                x.chave = chave;
                int resultado = 0;
                if (arvore)
                    resultado = pesquisa_b_estrela(&x, arvore);

                if (resultado)
                    printf("Registro encontrado: chave = %d\n", x.chave);
                else
                    printf("Registro não encontrado\n");
                print_counters_bstar();

                if (exibirChaves) {
                    for (int i = 6; i < argc; i++) {
                        tipo_registro_estrela x;
                        x.chave = atoi(argv[i]);
                        resultado = pesquisa_b_estrela(&x, arvore);

                        if (resultado)
                            printf("Registro encontrado: chave = %d\n", x.chave);
                        else
                            printf("Registro não encontrado\n");
                        print_counters_asi();
                    }
                }
                if (arvore)
                    libera_arvore_b_estrela(arvore);
            }
            break;
        default:
            printf("Método inválido! Use 1 a 4.\n");
            return 1;
    }
    return 0;
}
