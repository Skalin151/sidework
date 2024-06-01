#ifndef SIDEWORK_LIVROS_H
#define SIDEWORK_LIVROS_H

#define MAX_LIVROS 100
#define MAX_REQUISITANTES 100
#define MAX_DISTRITOS 100
#define MAX_CONCELHOS 100
#define MAX_FREGUESISAS 100
#define MAX_REQ_LIVROS 100
#define REQUISITADO 1
#define NAO_REQUISITADO 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "estruturas.h"
#include "requisicao.h"

int validarDataNascimento(char *data_nasc);
int validarIdFreguesia(char *id_freguesia);
Livro *buscarLivro(char *ISBN);
int calcularAlgoritmoControle(int id);


void adicionarLivro() {
    Livro *novoLivro = (Livro *)malloc(sizeof(Livro));
    if (novoLivro == NULL) {
        printf("Erro ao alocar memória para um novo livro.\n");
        return;
    }

    printf("Digite o ISBN do livro: ");
    scanf("%s", novoLivro->ISBN);
    printf("Digite o título do livro: ");
    scanf(" %[^\n]s", novoLivro->titulo);
    printf("Digite o autor do livro: ");
    scanf(" %[^\n]s", novoLivro->autor);
    printf("Digite a área do livro: ");
    scanf(" %[^\n]s", novoLivro->area);
    printf("Digite o ano de publicação do livro: ");
    scanf("%d", &novoLivro->ano_publicacao);

    novoLivro->status = NAO_REQUISITADO;
    novoLivro->id_requisitante = -1;
    novoLivro->qtd_requisicoes = 0;
    novoLivro->prox = NULL;

    int indice = hash(novoLivro->ISBN);
    novoLivro->prox = tabela_hash[indice];
    tabela_hash[indice] = novoLivro;

    printf("Livro adicionado com sucesso!\n");
}

// Função para gravar os livros no arquivo
void gravarLivros(FILE *arquivo) {
    for (int i = 0; i < MAX_LIVROS; i++) {
        Livro *temp = tabela_hash[i];
        while (temp != NULL) {
            fprintf(arquivo, "%s;%s;%s;%s;%d;%d;%d;%d\n", temp->ISBN, temp->titulo, temp->autor, temp->area, temp->ano_publicacao, temp->qtd_requisicoes, temp->status, temp->id_requisitante);
            temp = temp->prox;
        }
    }
}

// Função para listar os livros cadastrados
void listarLivros() {
    printf("\n### LISTA DE LIVROS ###\n");
    for (int i = 0; i < MAX_LIVROS; i++) {
        Livro *temp = tabela_hash[i];
        while (temp != NULL) {
            printf("ISBN: %s\n", temp->ISBN);
            printf("Título: %s\n", temp->titulo);
            printf("Autor: %s\n", temp->autor);
            printf("Área: %s\n", temp->area);
            printf("Ano de publicação: %d\n", temp->ano_publicacao);
            printf("Quantidade de vezes requisitado:%d\n", temp->qtd_requisicoes);
            printf("Status: %s\n", temp->status == REQUISITADO ? "Requisitado" : "Disponível");
            printf("\n");
            temp = temp->prox;
        }
    }
}

// Função para incluir um novo requisitante
void CriarRequisitante() {
    Requisitante *novo_requisitante = (Requisitante *)malloc(sizeof(Requisitante));
    if (novo_requisitante == NULL) {
        printf("Erro: Falha ao alocar memória para o novo requisitante.\n");
        return;
    }

    int id_requisitante;
    do {
        printf("Digite o ID do requisitante (9 dígitos, último sendo de controle): ");
        scanf("%d", &id_requisitante);
    } while (!validarIdRequisitante(id_requisitante));
    novo_requisitante->id_requisitante = id_requisitante;

    printf("Digite o nome do requisitante: ");
    scanf(" %[^\n]", novo_requisitante->nome);

    char data_nasc[11];
    do {
        printf("Digite a data de nascimento (dd-mm-aaaa): ");
        scanf("%s", data_nasc);
    } while (!validarDataNascimento(data_nasc));
    strcpy(novo_requisitante->data_nasc, data_nasc);

    char id_freguesia[7];
    do {
        printf("Digite o ID da freguesia (6 caracteres): ");
        scanf("%s", id_freguesia);
    } while (!validarIdFreguesia(id_freguesia));
    strcpy(novo_requisitante->id_freguesia, id_freguesia);

    novo_requisitante->novo = 1; // Indica que é um novo requisitante
    novo_requisitante->prox = NULL;

    int indice = novo_requisitante->id_requisitante % MAX_REQUISITANTES;

    if (lista_requisitantes[indice] == NULL) {
        lista_requisitantes[indice] = novo_requisitante;
    } else {
        Requisitante *temp = lista_requisitantes[indice];
        while (temp->prox != NULL) {
            temp = temp->prox;
        }
        temp->prox = novo_requisitante;
    }

    printf("Requisitante adicionado com sucesso.\n");
}

// Função para verificar a existência de um requisitante pelo ID
int verificarExistenciaRequisitante(int id_requisitante) {
    int indice = id_requisitante % MAX_REQUISITANTES;
    Requisitante *temp = lista_requisitantes[indice];
    while (temp != NULL) {
        if (temp->id_requisitante == id_requisitante) {
            return 1;
        }
        temp = temp->prox;
    }
    return 0;
}

// Função para validar a data de nascimento
int validarDataNascimento(char *data_nasc) {
    if (strlen(data_nasc) != 10) return 0;
    if (data_nasc[2] != '-' || data_nasc[5] != '-') return 0;
    for (int i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && !isdigit(data_nasc[i])) return 0;
    }
    return 1;
}

int validarIdFreguesia(char *id_freguesia) {
    return strlen(id_freguesia) == 6;
}

// Função para efetuar a requisição de um livro
void carregarRequisicoes() {
    FILE *arquivo = fopen("ISBN.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        Livro *livro = (Livro *)malloc(sizeof(Livro));
        if (livro == NULL) {
            printf("Erro: memoria insuficiente.\n");
            fclose(arquivo);
            return;
        }

        int id_requisitante = 0;
        if (sscanf(linha, "%s\t%[^\t]\t%[^\t]\t%[^\t]\t%d\t%d\t%d\t%d",
                   livro->ISBN, livro->titulo, livro->autor, livro->area, //
                   &livro->ano_publicacao, &livro->qtd_requisicoes, &livro->status, &id_requisitante) == 8) {
            livro->id_requisitante = id_requisitante;
        } else if (sscanf(linha, "%s\t%[^\t]\t%[^\t]\t%[^\t]\t%d\t%d\t%d",
                          livro->ISBN, livro->titulo, livro->autor, livro->area, //
                          &livro->ano_publicacao, &livro->qtd_requisicoes, &livro->status, &livro->qtd_requisicoes) == 7) {
            livro->id_requisitante = 0;
        }

        int index = hash(livro->ISBN);
        livro->prox = tabela_hash[index];
        tabela_hash[index] = livro;

        if (livro->status == REQUISITADO) {
            RequisicaoLivro *nova_requisicao = (RequisicaoLivro *)malloc(sizeof(RequisicaoLivro));
            if (nova_requisicao == NULL) {
                printf("Erro: Falha ao alocar memoria para a nova requisicao.\n");
                fclose(arquivo);
                return;
            }
            strcpy(nova_requisicao->ISBN, livro->ISBN);
            nova_requisicao->id_requisitante = livro->id_requisitante;
            nova_requisicao->prox = NULL;

            int indice = livro->id_requisitante % MAX_REQ_LIVROS;
            if (lista_requisicoes[indice] == NULL) {
                lista_requisicoes[indice] = nova_requisicao;
            } else {
                RequisicaoLivro *temp = lista_requisicoes[indice];
                while (temp->prox != NULL) {
                    temp = temp->prox;
                }
                temp->prox = nova_requisicao;
            }
        }
    }

    fclose(arquivo);
}

void salvarRequisicoes() {
    FILE *file = fopen("ISBN.txt", "w");
    if (!file) {
        printf("Erro: Não foi possível abrir o arquivo ISBN.txt para escrita\n");
        return;
    }

    Livro *livro = obterPrimeiroLivro();
    while (livro != NULL) {
        if (livro->status == REQUISITADO) {
            fprintf(file, "%s\t%s\t%s\t%s\t%d\t%d\t%d\t%d\n",
                    livro->ISBN, livro->titulo, livro->autor, livro->area,
                    livro->ano_publicacao, livro->qtd_requisicoes, livro->status, livro->id_requisitante);
        } else {
            fprintf(file, "%s\t%s\t%s\t%s\t%d\t%d\t%d\n",
                    livro->ISBN, livro->titulo, livro->autor, livro->area,
                    livro->ano_publicacao, livro->qtd_requisicoes, livro->status);
        }
        livro = obterProximoLivro(livro);
    }

    fclose(file);
}

void efetuarRequisicaoLivro(char *ISBN, int id_requisitante) {
    int indice = hashISBN(ISBN);
    Livro *livro = tabela_hash[indice];

    // Percorre a lista ligada de livros para encontrar o livro com o ISBN correspondente
    while (livro != NULL) {
        if (strcmp(livro->ISBN, ISBN) == 0) {
            livro->status = REQUISITADO;
            livro->id_requisitante = id_requisitante;
            printf("Livro requisitado com sucesso!\n");
            return;
        }
        livro = livro->prox;
    }
    printf("Livro não encontrado.\n");
}

// Funções de verificação de requisitante e busca de livro (exemplo)
Livro *buscarLivro(char *ISBN) {
    int indice = hash(ISBN);
    Livro *livro = tabela_hash[indice];

    while (livro != NULL) {
        if (strcmp(livro->ISBN, ISBN) == 0) {
            return livro;
        }
        livro = livro->prox;
    }
    return NULL;
}

// Função para devolver um livro requisitado
void devolverLivro() {
    char ISBN[14];
    printf("Digite o ISBN do livro a ser devolvido: ");
    scanf("%s", ISBN);

    int indice = hashISBN(ISBN);
    Livro *livro = tabela_hash[indice];

    // Percorre a lista ligada de livros para encontrar o livro com o ISBN correspondente
    while (livro != NULL) {
        if (strcmp(livro->ISBN, ISBN) == 0) {
            if (livro->status == REQUISITADO) {
                // Atualiza o status do livro e limpa o id do requisitante
                livro->status = NAO_REQUISITADO;
                livro->id_requisitante = -1;
                printf("Livro devolvido com sucesso!\n");
            } else {
                printf("O livro já está disponível na biblioteca.\n");
            }
            return;
        }
        livro = livro->prox;
    }
    printf("Livro não encontrado.\n");
}

// Função para listar os livros requisitados
void listarLivrosRequisitados() {
    printf("===== Lista de Livros Requisitados =====\n");
    for (int i = 0; i < MAX_LIVROS; i++) {
        Livro *livro = tabela_hash[i];
        while (livro != NULL) {
            if (livro->status == REQUISITADO) {
                printf("ISBN: %s\n", livro->ISBN);
                printf("Título: %s\n", livro->titulo);
                printf("Autor: %s\n", livro->autor);
                printf("Requisitante: %d\n", livro->id_requisitante);
                printf("===========================\n");
            }
            livro = livro->prox;
        }
    }
}

// Implementação da função getIndex
int getIndex(int id) {
    // Lógica para encontrar o índice do requisitante com o ID fornecido
    // Esta é apenas uma implementação de exemplo, substitua com sua lógica real
    return id % 10; // Exemplo simples: apenas retorna o módulo 10 do ID
}

//Logs
void registarErro(const char *mensagemErro) {
    FILE *arquivoLog;
    arquivoLog = fopen("logs.txt", "a");
    if (arquivoLog == NULL) {
        printf("Erro ao abrir arquivo de log: logs.txt\n");
        exit(1);
    }
    char mensagemCompleta[1024];
    sprintf(mensagemCompleta, "%s\n", mensagemErro);
    fprintf(arquivoLog, "%s", mensagemCompleta);
    fclose(arquivoLog);
}

// Função para gravar os requisitantes no arquivo
void gravarRequisitantes(FILE *arquivo) {
    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *req = lista_requisitantes[i];
        while (req != NULL) {
            fprintf(arquivo, "ID: %d\n", req->id_requisitante);
            fprintf(arquivo, "Nome: %s\n", req->nome);
            fprintf(arquivo, "Data de Nascimento: %s\n", req->data_nasc);
            fprintf(arquivo, "ID da Freguesia: %s\n", req->id_freguesia);
            fprintf(arquivo, "Status: %d\n", req->status);
            fprintf(arquivo, "Novo: %d\n", req->novo);
            fprintf(arquivo, "\n");
            req = req->prox;
        }
    }
}

#endif //SIDEWORK_LIVROS_H
