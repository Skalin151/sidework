#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <time.h>
#include <math.h>

#include "requisicao.h"
#include "livros.h"
#include "hashing.h"

#define MAX_LIVROS 100
#define MAX_REQUISITANTES 100
#define MAX_DISTRITOS 100
#define MAX_CONCELHOS 100
#define MAX_FREGUESISAS 100
#define MAX_REQ_LIVROS 100
#define REQUISITADO 1
#define NAO_REQUISITADO 0

// Protótipos de funções
Livro *buscarLivro(char *ISBN);
int hash(const char *ISBN);
int hashISBN(const char *isbn);
int verificarExistenciaRequisitante(int id_requisitante);
int validarIdRequisitante(int id_requisitante);
int validarDataNascimento(char *data_nasc);
int validarIdFreguesia(char *id_freguesia);
int getIndex(int id);
int calcularAlgoritmoControle(int id);
void efetuarRequisicaoLivro(char *ISBN, int id_requisitante);
void adicionarLivro();
void gravarLivros(FILE *arquivo);
void gravarRequisitantes(FILE *arquivo);
void listarLivros();
void CriarRequisitante();
void listarRequisitantes();
void carregarRequisitantes();
void imprimirRequisitantes();
void listarUsuariosMaisRequisitantes();
void devolverLivro();
void listarLivrosRequisitados();
void listarRequisicoes();
void gravarRequisicoes(FILE *arquivo);
void carregarRequisicoes();
void salvarRequisicoes();
void Areamaisrepetida();
void encontrarLivroMaisRecente();
void encontrarAreaMaisRequisitada();
void adicionarLivroALista();
void listarLivrosRequisitadosPorArea();
size_t calcularTamanhoTotal();
float calcularMediaIdades();
int contarPessoasIdadeSuperior(int valor_limite);
int determinarIdadeMaisRequisitantes(int *idade_mais_requisitantes);
RequisicaoLivro *criarRequisicaoLivro(char *ISBN, int id_requisitante);
void adicionarRequisicaoLivro(RequisicaoLivro **lista_requisicoes, RequisicaoLivro *nova_requisicao);
void consultarLogsRequisicoes();
void LogsPessoais();
void listarPessoasNuncaRequisitaramLivro(Requisitante *lista_requisitantes);

int main() {
    srand(time(NULL));
    setlocale(LC_ALL, "Portuguese");
    int opcao;
    carregarRequisitantes();
    carregarRequisicoes();

    while (1) {
        printf("\n### MENU ###\n");
        printf("1. Adicionar um livro\n");
        printf("2. Buscar um livro por ISBN\n");
        printf("3. Consultar livros da biblioteca\n");
        printf("4. Gerir a lista de requisitantes\n");
        printf("5. Gerir as requisicoes de livros\n");
        printf("6. Gravar todos os dados em ficheiros de texto\n");
        printf("7. Criar um ficheiro XML.\n");
        printf("8. Determinar o espaco ocupado pela estrutura de dados.\n");
        printf("9. Limpar a memoria.\n");
        printf("10. Sair\n");
        printf("\nEscolha uma opcao: \n");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                adicionarLivro();
                break;
            case 2: {
                char ISBN[14];
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
            case 3: {
                int sub_opcao;
                while (1) {
                    printf("\n### CONSULTAR LIVROS ###\n");
                    printf("1. Listar todos os livros\n");
                    printf("2. Area com mais livros\n");
                    printf("3. Livro com o ano de publicação mais recente\n");
                    printf("4. Area com mais livros requisitados\n");
                    printf("5. Voltar ao menu principal\n");
                    printf("Escolha uma opcao: ");
                    scanf("%d", &sub_opcao);
                    switch (sub_opcao) {
                        case 1:
                            listarLivros();
                            break;
                        case 2:
                            Areamaisrepetida();
                            break;
                        case 3:
                            encontrarLivroMaisRecente();
                            break;
                        case 4:
                            encontrarAreaMaisRequisitada();
                            break;
                        case 5:
                            sub_opcao = 0;
                            break;
                        default:
                            printf("Opcao invalida.\n");
                            break;
                    }
                    if (sub_opcao == 0) {
                        break;
                    }
                }
                break;
            }
            case 4: {
                int sub_opcao;
                while (1) {
                    printf("\n### GERIR REQUISITANTES ###\n");
                    printf("1. Criar novo requisitante\n");
                    printf("2. Verificar existencia de requisitante\n");
                    printf("3. Listar todos os requisitantes\n");
                    printf("4. Listar usuarios mais requesitantes\n");
                    printf("5. Listar usuarios requesitantes que sao mais velhos\n");
                    printf("6. Idade media dos requisitnates\n");
                    printf("7. Quantidade de pessoas acima de uma certa idade\n");
                    printf("8. Idade com mais requisitantes\n");
                    printf("9. Mostrar pessoas que nunca requisitaram\n");
                    printf("10. Voltar ao menu principal\n");
                    printf("Escolha uma opção: \n");
                    scanf("%d", &sub_opcao);
                    switch (sub_opcao) {
                        case 1:
                            CriarRequisitante();
                            break;
                        case 2: {
                            int sub_opcao;
                            printf("\n### Como procurar requisitante ###\n");
                            printf("1. ISBN\n");
                            printf("2. Nome\n");
                            printf("3. Voltar ao menu anterior\n");
                            scanf("%d", &sub_opcao);
                            switch (sub_opcao) {
                                case 1: {
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
                                case 2: {
                                    char nomeProcurado[200];
                                    printf("Digite o nome do requisitante (ou 'sair' para encerrar): ");
                                    scanf("%d", nomeProcurado);
                                    if (fgets(nomeProcurado, sizeof(nomeProcurado), stdin) == NULL) {
                                        printf("Erro ao ler a entrada. Tente novamente.\n");
                                        continue;
                                    }

                                    // Remover o caractere de nova linha (\n) do final da string
                                    nomeProcurado[strcspn(nomeProcurado, "\n")] = 0;

                                    // Verificar se o usuário deseja sair
                                    if (strcmp(nomeProcurado, "sair") == 0) {
                                        break;
                                    }

                                    // Verificar a existência de um requisitante pelo nome
                                    if (verificarExistenciaRequisitantePorNome(nomeProcurado)) {
                                        printf("O requisitante %s existe na lista.\n", nomeProcurado);
                                    } else {
                                        printf("O requisitante %s não existe na lista.\n", nomeProcurado);
                                    }
                                    break;
                                }
                                case 3:
                                    sub_opcao = 0;
                                    break;
                                default:
                                    printf("Opcao invalida.\n");
                                    break;
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
                            listarRequisitantesMaisVelhos();
                            break;
                        case 6:
                            calcularMediaIdades();
                            break;
                        case 7:
                            int limite;
                            printf("Introduza a idade limite: ");
                            scanf("%d", &limite);

                            int numero_pessoas = contarPessoasIdadeSuperior(limite);
                            printf("O numero de pessoas com idade superior a %d anos e: %d\n", limite, numero_pessoas);
                            break;
                        case 8:
                            int idade_mais_requisitantes;
                            int contagem = determinarIdadeMaisRequisitantes(&idade_mais_requisitantes);

                            printf("A idade com mais requisitantes é: %d anos, com um total de %d requisitantes.\n", idade_mais_requisitantes, contagem);
                            break;
                        case 9:
                            listarPessoasNuncaRequisitaramLivro(lista_requisitantes);
                            break;
                        case 10:
                            sub_opcao = 0;
                            break;
                        default:
                            printf("Opcao invalida.\n");
                            break;
                    }
                    if (sub_opcao == 0) {
                        break;
                    }
                }
                break;
            }
            case 5: {
                int sub_opcao;
                while (1) {
                    printf("\n### GERIR REQUISICOES ###\n");
                    printf("1. Efetuar requisicao de um livro\n");
                    printf("2. Devolver um livro\n");
                    printf("3. Listar livros requisitados\n");
                    printf("4. Listar livros mais requisitados\n");
                    printf("5. Livros Requisitados organizados por area\n");
                    printf("6. Consultar requisições de livros\n");
                    printf("7. Consultar requisições de uma certa pessoa\n");
                    printf("8. Voltar ao menu principal\n");
                    printf("Escolha uma opcao: ");
                    scanf("%d", &sub_opcao);
                    switch (sub_opcao) {
                        case 1: {
                            char ISBN[14];
                            int id_requisitante;
                            printf("Digite o ISBN do livro: ");
                            scanf("%s", ISBN);
                            printf("Digite o ID do requisitante: ");
                            scanf("%d", &id_requisitante);
                            efetuarRequisicaoLivro(ISBN, id_requisitante);
                            salvarRequisicoes();
                            break;
                        }
                        case 2:
                            devolverLivro();
                            break;
                        case 3:
                            listarLivrosRequisitados();
                            break;
                        case 4:
                            LivroMaisRequisitado();
                            break;
                        case 5:
                            listarLivrosRequisitadosPorArea();
                            break;
                        case 6:
                            consultarLogsRequisicoes();
                            break;
                        case 7:
                            LogsPessoais();
                            break;
                        case 8:
                            sub_opcao = 0;
                            break;
                        default:
                            printf("Opcao invalida.\n");
                            break;
                    }
                    if (sub_opcao == 0) {
                        break;
                    }
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
            case 7: {
                char nomeArquivo[256];
                printf("Digite o nome do arquivo XML para salvar os dados: ");
                scanf("%s", nomeArquivo);

                // Gerar o arquivo XML
                gerarArquivoXML(nomeArquivo);

                printf("Arquivo XML gerado com sucesso.\n");
                break;
            }
            case 8: {
                size_t tamanho_total = calcularTamanhoTotal();
                printf("\nO tamanho total das estruturas de dados e : %zu bytes\n", tamanho_total);
                break;
            }
            case 9: {
                // Liberar a memória dos livros
                for (int i = 0; i < MAX_LIVROS; i++) {
                    Livro *livro = tabela_hash[i];
                    while (livro != NULL) {
                        Livro *temp = livro;
                        livro = livro->prox;
                        free(temp);
                    }
                    tabela_hash[i] = NULL;
                }

                // Liberar a memória dos requisitantes
                for (int i = 0; i < MAX_REQUISITANTES; i++) {
                    Requisitante *requisitante = lista_requisitantes[i];
                    while (requisitante != NULL) {
                        Requisitante *temp = requisitante;
                        requisitante = requisitante->prox;
                        free(temp);
                    }
                    lista_requisitantes[i] = NULL;
                }

                // Liberar a memória das requisições de livros
                for (int i = 0; i < MAX_REQ_LIVROS; i++) {
                    RequisicaoLivro *requisicao = lista_requisicoes[i];
                    while (requisicao != NULL) {
                        RequisicaoLivro *temp = requisicao;
                        requisicao = requisicao->prox;
                        free(temp);
                    }
                    lista_requisicoes[i] = NULL;
                }

                printf("Memoria liberada com sucesso :) \n");
                break;
            }
            case 10: {
                printf("A sair do programa...\n");
                exit(0);
            }
            default:
                printf("Opcao invalida.\n");
                break;
        }
    }
    return 0;
}
