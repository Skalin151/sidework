#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "requisicao.h"
#include "livros.h"

#define MAX_LIVROS 100
#define MAX_REQUISITANTES 100
#define MAX_DISTRITOS 100
#define MAX_CONCELHOS 100
#define MAX_FREGUESISAS 100
#define MAX_REQ_LIVROS 100
#define REQUISITADO 1
#define NAO_REQUISITADO 0

// Protótipos de funções
int hash(char *ISBN);
int hashISBN(char *isbn);
void adicionarLivro();
Livro *buscarLivro(char *ISBN);
void gravarLivros(FILE *arquivo);
void gravarRequisitantes(FILE *arquivo);
void listarLivros();
void CriarRequisitante();
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
        printf("5. Gerir as requisicoes de livros\n");
        printf("6. Gravar todos os dados em ficheiros de texto\n");
        printf("7. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                adicionarLivro();
                break;
            case 2: {
                char ISBN[14];
                printf("Digite o ISBN do livro (13 digitos): ");
                do {
                    printf("Digite o ISBN do livro (13 digitos): ");
                    scanf("%s", ISBN);
                if (strlen(ISBN) != 13) {
                    printf("ISBN invalido. Deve conter exatamente 13 digitos.\n");
                    }
                } while (strlen(ISBN) != 13);

                Livro *livro = buscarLivro(ISBN);
                if (livro != NULL) {
                    printf("Livro encontrado: %s\n", livro->titulo);
                    printf("Autor: %s\n", livro->autor);
                    printf("Area: %s\n", livro->area);
                    printf("Ano de Publicacao: %s\n", livro->ano_publicacao);

                } else {
                    printf("Livro não encontrado.\n");
                }
                break;
            }
            case 3:
                listarLivros();
                break;
            case 4: {
                int sub_opcao;
                while (1) {
                    printf("\n### GERIR REQUISITANTES ###\n");
                    printf("1. Criar novo requisitante\n");
                    printf("2. Verificar existência de requisitante\n");
                    printf("3. Listar todos os requisitantes\n");
                    printf("4. Listar usuarios mais requesitantes\n");
                    printf("5. Voltar ao menu principal\n");
                    printf("Escolha uma opção: ");
                    scanf("%d", &sub_opcao);
                    switch (sub_opcao) {
                        case 1:
                            CriarRequisitante();
                            break;
                        case 2: {
                            int id_requisitante;
                            printf("Digite o ID do requisitante a ser verificado: ");
                            scanf("%d", &id_requisitante);
                            if (verificarExistenciaRequisitante(id_requisitante)) {
                                printf("Requisitante encontrado.\n");
                            } else {
                                printf("Requisitante nao encontrado.\n");
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
                printf("3. Listar livros mais requisitados\n");
                printf("5. Voltar ao menu principal\n");
                printf("Escolha uma opcao: ");
                scanf("%d", &sub_opcao);
                switch (sub_opcao) {
                    case 1:
                        printf("Digite o ISBN do livro: ");
                        scanf("%s", ISBN);
                        printf("Digite o ID do requisitante: ");
                        scanf("%d", &id_requisitante);
                        efetuarRequisicaoLivro(ISBN, id_requisitante);
                        //salvarRequisicoes();
                        break;
                    case 2:
                        devolverLivro();
                        //salvarRequisicoes();
                        break;
                    case 3:
                        listarLivrosRequisitados();
                        break;
                    case 4:
                        break;
                    case 5:
                        return;
                    default:
                        printf("Opção invalida.\n");
                }
                break;
            }
            case 6: {
                FILE *arquivo = fopen("livros.txt", "w");
                if (arquivo == NULL) {
                    printf("Erro ao abrir o arquivo para gravar os livros.\n");
                    exit(1);
                }
                gravarLivros(arquivo);
                fclose(arquivo);
                printf("Dados dos livros gravados com sucesso no arquivo livros.txt'.\n");

                FILE *requisitantes_arquivo = fopen("requisitantesnovos.txt", "w");
                if (requisitantes_arquivo == NULL) {
                    printf("Erro ao abrir o arquivo de requisitantes.\n");
                    return 1;
                }
                gravarRequisitantes(requisitantes_arquivo);
                fclose(requisitantes_arquivo);
                printf("Dados dos requisitantes gravados com sucesso no arquivo 'requisitantesnovos.txt'.\n");

                break;
            }
            case 7:
                printf("A sair do programa...\n");
                exit(0);
            default:
                printf("Opcao invalida.\n");
        }
    }
    return 0;
}


