#ifndef SIDEWORK_LIVROS_H
#define SIDEWORK_LIVROS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "requicao.h"

int validarDataNascimento(char *data_nasc);
int validarIdFreguesia(char *id_freguesia);
Livro *buscarLivro(char *ISBN);
int calcularAlgoritmoControle(int id);


void adicionarLivro() {
    Livro *novo_livro = (Livro *)malloc(sizeof(Livro));
    if (novo_livro == NULL) {
        printf("Erro: Falha ao alocar memória para o novo livro.\n");
        return;
    }

    printf("Digite o ISBN do livro: ");
    scanf("%s", novo_livro->ISBN);

    if (buscarLivro(novo_livro->ISBN) != NULL) {
        printf("Erro: Já existe um livro com o ISBN %s.\n", novo_livro->ISBN);
        free(novo_livro);
        return;
    }

    printf("Digite o título do livro: ");
    scanf(" %[^\n]", novo_livro->titulo);
    printf("Digite o autor do livro: ");
    scanf(" %[^\n]", novo_livro->autor);
    printf("Digite a área do livro: ");
    scanf(" %[^\n]", novo_livro->area);
    printf("Digite o ano de publicação do livro: ");
    scanf("%d", &novo_livro->ano_publicacao);
    novo_livro->status = NAO_REQUISITADO;
    novo_livro->prox = NULL;

    int indice = hash(novo_livro->ISBN);

    if (tabela_hash[indice] == NULL) {
        tabela_hash[indice] = novo_livro;
    } else {
        Livro *temp = tabela_hash[indice];
        while (temp->prox != NULL) {
            temp = temp->prox;
        }
        temp->prox = novo_livro;
    }

    printf("Livro adicionado com sucesso.\n");
}

// Função para gravar os livros no arquivo
void gravarLivros(FILE *arquivo) {
    for (int i = 0; i < MAX_LIVROS; i++) {
        Livro *temp = tabela_hash[i];
        while (temp != NULL) {
            fprintf(arquivo, "%s;%s;%s;%s;%d;%d;%d\n", temp->ISBN, temp->titulo, temp->autor, temp->area, temp->ano_publicacao, temp->status, temp->id_requisitante);
            temp = temp->prox;
        }
    }
}

// Função para carregar os livros do arquivo
void carregarLivros(FILE *arquivo) {
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
    char ISBN[14];
    while (fscanf(arquivo, " %[^\n]", ISBN) != EOF) {
        Livro *livro = (Livro *)malloc(sizeof(Livro));
        if (livro == NULL) {
            printf("Erro: memória insuficiente.\n");
            return;
        }
        strcpy(livro->ISBN, ISBN);
        fscanf(arquivo, " %[^\n]", livro->titulo);
        fscanf(arquivo, " %[^\n]", livro->autor);
        fscanf(arquivo, " %[^\n]", livro->area);
        fscanf(arquivo, "%d", &livro->ano_publicacao);
        fscanf(arquivo, "%d", &livro->status);

        int index = hash(ISBN);
        livro->prox = tabela_hash[index];
        tabela_hash[index] = livro;
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
            printf("Status: %s\n", temp->status == REQUISITADO ? "Requisitado" : "Disponível");
            printf("\n");
            temp = temp->prox;
        }
    }
}

// Função para incluir um novo requisitante
void incluirRequisitante() {
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


// Função para listar os requisitantes






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
    FILE *file = fopen("ISBN.txt", "r");
    if (!file) {
        printf("Erro: Não foi possível abrir o arquivo ISBN.txt\n");
        return;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), file)) {
        Livro *livro = (Livro *)malloc(sizeof(Livro));
        if (livro == NULL) {
            printf("Erro: Falha ao alocar memória para o livro.\n");
            fclose(file);
            return;
        }

        char isbn[14];
        int status, id_requisitante = 0;

        if (sscanf(linha, "%s %d %d", isbn, &status, &id_requisitante) == 3) {
            livro->status = status;
            livro->id_requisitante = id_requisitante;
        } else if (sscanf(linha, "%s %d", isbn, &status) == 2) {
            livro->status = status;
            livro->id_requisitante = 0;
        }

        strcpy(livro->ISBN, isbn);
        adicionarLivroLista(livro);

        if (livro->status == REQUISITADO) {
            RequisicaoLivro *nova_requisicao = (RequisicaoLivro *)malloc(sizeof(RequisicaoLivro));
            if (nova_requisicao == NULL) {
                printf("Erro: Falha ao alocar memória para a nova requisição.\n");
                fclose(file);
                return;
            }
            strcpy(nova_requisicao->ISBN, isbn);
            nova_requisicao->id_requisitante = id_requisitante;
            nova_requisicao->prox = NULL;

            int indice = id_requisitante % MAX_REQ_LIVROS;
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

    fclose(file);
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
            fprintf(file, "%s %d %d\n", livro->ISBN, REQUISITADO, livro->id_requisitante);
        } else {
            fprintf(file, "%s %d\n", livro->ISBN, NAO_REQUISITADO);
        }
        livro = obterProximoLivro(livro);
    }

    fclose(file);
}



void efetuarRequisicaoLivro() {
    char ISBN[14];
    int id_requisitante;

    printf("Digite o ISBN do livro: ");
    scanf("%s", ISBN);
    printf("Digite o ID do requisitante: ");
    scanf("%d", &id_requisitante);

    if (!verificarExistenciaRequisitante(id_requisitante)) {
        printf("Erro: Requisitante não encontrado.\n");
        return;
    }

    Livro *livro = buscarLivro(ISBN);
    if (livro == NULL) {
        printf("Erro: Livro não encontrado.\n");
        return;
    }

    if (livro->status == REQUISITADO) {
        printf("Erro: Livro já está requisitado.\n");
        return;
    }

    livro->status = REQUISITADO;
    livro->id_requisitante = id_requisitante;

    RequisicaoLivro *nova_requisicao = (RequisicaoLivro *)malloc(sizeof(RequisicaoLivro));
    if (nova_requisicao == NULL) {
        printf("Erro: Falha ao alocar memória para a nova requisição.\n");
        return;
    }

    strcpy(nova_requisicao->ISBN, ISBN);
    nova_requisicao->id_requisitante = id_requisitante;
    nova_requisicao->prox = NULL;

    int indice = id_requisitante % MAX_REQ_LIVROS;
    if (lista_requisicoes[indice] == NULL) {
        lista_requisicoes[indice] = nova_requisicao;
    } else {
        RequisicaoLivro *temp = lista_requisicoes[indice];
        while (temp->prox != NULL) {
            temp = temp->prox;
        }
        temp->prox = nova_requisicao;
    }

    printf("Requisição realizada com sucesso.\n");
}

// Funções de verificação de requisitante e busca de livro (exemplo)
Livro* buscarLivro(char* ISBN) {
    // Função para buscar livro (exemplo)
    Livro *livro = (Livro *)malloc(sizeof(Livro));
    if (livro == NULL) {
        printf("Erro: Falha ao alocar memória para o livro.\n");
        return NULL;
    }
    strcpy(livro->ISBN, ISBN);
    livro->status = NAO_REQUISITADO; // Exemplo: sempre disponível
    livro->id_requisitante = 0;
    return livro;
}


// Função para devolver um livro requisitado
void devolverLivro() {
    char ISBN[14];
    printf("Digite o ISBN do livro que deseja devolver: ");
    scanf("%s", ISBN);
    Livro *livro = buscarLivro(ISBN);
    if (livro == NULL) {
        printf("Livro não encontrado.\n");
        return;
    }
    if (livro->status == NAO_REQUISITADO) {
        printf("Este livro não está requisitado.\n");
        return;
    }
    livro->status = NAO_REQUISITADO;
    printf("Livro devolvido com sucesso.\n");
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



// Função para gravar os requisitantes no arquivo
void gravarRequisitantes() {
    FILE *arquivo = fopen("requisitantesnovos.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
    Requisitante *req = requisitantes_novos;
    while (req != NULL) {
        fprintf(arquivo, "%d\n", req->id_requisitante);
        fprintf(arquivo, "%s\n", req->nome);
        fprintf(arquivo, "%s\n", req->data_nasc);
        fprintf(arquivo, "%s\n", req->id_freguesia);
        req = req->prox;
    }
    fclose(arquivo);
}

void listarRequisicoes() {
    printf("\n### LISTA DE REQUISIÇÕES ###\n");
    for (int i = 0; i < MAX_REQ_LIVROS; i++) {
        RequisicaoLivro *temp = lista_requisicoes[i];
        while (temp != NULL) {
            printf("ISBN: %s\n", temp->ISBN);
            printf("ID Requisitante: %d\n", temp->id_requisitante);
            printf("\n");
            temp = temp->prox;
        }
    }
}

void gravarRequisicoes(FILE *arquivo) {
    for (int i = 0; i < MAX_REQ_LIVROS; i++) {
        RequisicaoLivro *temp = lista_requisicoes[i];
        while (temp != NULL) {
            fprintf(arquivo, "%s;%d\n", temp->ISBN, temp->id_requisitante);
            temp = temp->prox;
        }
    }
}

#endif //SIDEWORK_LIVROS_H
