#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "requicao.h"
#include "livros.h"
#include "estruturas.h"

// Protótipos de funções
int hash(char *ISBN);
void adicionarLivro();
Livro *buscarLivro(char *ISBN);
void gravarLivros(FILE *arquivo);
void gravarRequisitantes(FILE *arquivo);
void listarLivros();
void incluirRequisitante();
int verificarExistenciaRequisitante(int id_requisitante);
void listarRequisitantes();
void carregarRequisitantes();
void imprimirRequisitantes();
void listarUsuariosMaisRequisitantes();
int validarIdRequisitante(int id_requisitante);
int validarDataNascimento(char *data_nasc);
int validarIdFreguesia(char *id_freguesia);
void efetuarRequisicaoLivro(char *ISBN, int id_requisitante);
void devolverLivro();
void listarLivrosRequisitados();
int getIndex(int id);
int calcularAlgoritmoControle(int id);
void listarRequisicoes();
void gravarRequisicoes(FILE *arquivo);
void carregarRequisicoes();
void salvarRequisicoes();
void registarErro(const char *mensagem);

int main() {
    setlocale(LC_ALL, "Portuguese");
    int opcao;
    carregarRequisitantes();
    carregarRequisicoes();

    while (1) {
        printf("\n### MENU ###\n");
        printf("1. Adicionar um livro\n");
        printf("2. Buscar um livro por ISBN\n");
        printf("3. Listar todos os livros da biblioteca\n");
        printf("4. Gerir a lista de requisitantes\n");
        printf("5. Gerir as requisições de livros\n");
        printf("6. Gravar todos os dados em ficheiros de texto\n");
        printf("7. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                adicionarLivro();
                break;
            case 2: {
                char ISBN[14];
                printf("Digite o ISBN do livro: ");
                scanf("%s", ISBN);
                Livro *livro = buscarLivro(ISBN);
                if (livro != NULL) {
                    printf("Livro encontrado: %s\n", livro->titulo);
                    registarErro("Livro encontrado.\n");
                } else {
                    printf("Livro não encontrado.\n");
                    registarErro("Livro não encontrado.\n");
                }
                break;
            }
            case 3:{
                int sub_opcao;
                while (1) {
                    printf("\n### CONSULTAR LIVROS ###\n");
                    printf("1. Listar todos os livros");
                    printf("2. Area com mais livros");
                    scanf("%d", &sub_opcao);
                    switch (sub_opcao) {
                        case 1:
                            listarLivros();
                            break;
                        case 2:
                            break;
                    }
                }
            }
                break;
            case 4: {
                int sub_opcao;
                while (1) {
                    printf("\n### GERIR REQUISITANTES ###\n");
                    printf("1. Incluir novo requisitante\n");
                    printf("2. Verificar existência de requisitante\n");
                    printf("3. Listar todos os requisitantes\n");
                    printf("4. Listar usuarios mais requesitantes\n");
                    printf("5. Voltar ao menu principal\n");
                    printf("Escolha uma opção: ");
                    scanf("%d", &sub_opcao);
                    switch (sub_opcao) {
                        case 1:
                            incluirRequisitante();
                            break;
                        case 2: {
                            int id_requisitante;
                            printf("Digite o ID do requisitante a ser verificado: ");
                            scanf("%d", &id_requisitante);
                            if (verificarExistenciaRequisitante(id_requisitante)) {
                                printf("Requisitante encontrado.\n");
                                registarErro("Requisitante encontrado.\n");
                            } else {
                                printf("Requisitante não encontrado.\n");
                                registarErro("Requisitante não encontrado.\n");
                            }
                            break;
                        }
                        case 3:
                            imprimirRequisitantes();
                            break;
                        case 4:
                            listarUsuariosMaisRequisitantes();
                            break;
                        case 5:
                            return;
                        default:
                            printf("Opcao invalida.\n");
                            registarErro("Opcao invalida.\n");
                    }
                    if (sub_opcao == 5) {
                        break;
                    }
                }
                break;
            }

            case 5: {
                int sub_opcao;
                char ISBN[14];
                int id_requisitante;
                printf("\n### GERIR REQUISIÇÕES ###\n");
                printf("1. Efetuar requisição de um livro\n");
                printf("2. Devolver um livro\n");
                printf("3. Listar livros requisitados\n");
                printf("4. Listar livros mais requisitados\n");
                printf("5. Voltar ao menu principal\n");
                printf("Escolha uma opção: ");
                scanf("%d", &sub_opcao);
                switch (sub_opcao) {
                    case 1:
                        printf("Digite o ISBN do livro: ");
                        scanf("%s", ISBN);
                        printf("Digite o ID do requisitante: ");
                        scanf("%d", &id_requisitante);
                        efetuarRequisicaoLivro(ISBN, id_requisitante);
                        salvarRequisicoes();
                        break;
                    case 2:
                        devolverLivro();
                        salvarRequisicoes();
                        break;
                    case 3:
                        listarLivrosRequisitados();
                        break;
                    case 4:
                        
                        break;
                    case 5:
                        return;
                    default:
                        printf("Opção inválida.\n");
                }
                break;
            }
            case 6: {
                FILE *arquivo = fopen("livros.txt", "w");
                if (arquivo == NULL) {
                    printf("Erro ao abrir o arquivo para gravar os livros.\n");
                    registarErro("Erro ao abrir o arquivo para gravar os livros.\n");
                    exit(1);
                }
                gravarLivros(arquivo);
                fclose(arquivo);
                printf("Dados dos livros gravados com sucesso no arquivo 'livros.txt'.\n");
                registarErro("Dados dos livros gravados com sucesso no arquivo 'livros.txt'.\n");

                FILE *requisitantes_arquivo = fopen("requisitantesnovos.txt", "w");
                if (requisitantes_arquivo == NULL) {
                    printf("Erro ao abrir o arquivo de requisitantes.\n");
                    registarErro("Erro ao abrir o arquivo de requisitantes.\n");
                    return 1;
                }
                gravarRequisitantes(requisitantes_arquivo);
                fclose(requisitantes_arquivo);
                printf("Dados dos requisitantes gravados com sucesso no arquivo 'requisitantesnovos.txt'.\n");
                registarErro("Dados dos requisitantes gravados com sucesso no arquivo 'requisitantesnovos.txt'.\n");

                break;
            }
            case 7:
                printf("A sair do programa...\n");
                exit(0);
            default:
                printf("Opção invalida.\n");
                registarErro("Opção invalida.\n");
        }
    }
    return 0;
}