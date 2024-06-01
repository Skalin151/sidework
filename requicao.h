#ifndef SIDEWORK_REQUICAO_H
#define SIDEWORK_REQUICAO_H
#define MAX_LIVROS 100
#define MAX_REQUISITANTES 100
#define MAX_DISTRITOS 100
#define MAX_CONCELHOS 100
#define MAX_FREGUESIAS 100
#define MAX_REQ_LIVROS 100
#define REQUISITADO 1
#define NAO_REQUISITADO 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include "estruturas.h"
// Tabela de hash para os livros
Livro *tabela_hash[MAX_LIVROS];

// Lista ligada para os requisitantes
Requisitante *lista_requisitantes[MAX_REQUISITANTES];
Requisitante *requisitantes_novos = NULL;

// Lista ligada para as requisições de livros
RequisicaoLivro *lista_requisicoes[MAX_REQ_LIVROS];

// Vetores para armazenar distritos, concelhos e freguesias
Distrito distritos[MAX_DISTRITOS];
Concelho concelhos[MAX_CONCELHOS];
Freguesia freguesias[MAX_FREGUESIAS];

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

int calcularAlgoritmoControle(int id) {
    int soma = 0;
    int multiplicador = 2;
    while (id > 0) {
        int digito = id % 10;
        soma += digito * multiplicador;
        multiplicador++;
        id /= 10;
    }
    return (11 - (soma % 11)) % 10;
}

void carregarRequisitantes() {
    FILE *arquivo = fopen("Requisitantes.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de requisitantes.\n");
        registarErro("Erro ao abrir o arquivo de requisitantes.\n");
        return;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        Requisitante *novo_requisitante = (Requisitante *)malloc(sizeof(Requisitante));
        if (novo_requisitante == NULL) {
            printf("Erro: Falha ao alocar memória para o requisitante.\n");
            registarErro("Erro: Falha ao alocar memória para o requisitante.\n");
            fclose(arquivo);
            return;
        }

        // Inicializa a estrutura para evitar dados incorretos
        memset(novo_requisitante, 0, sizeof(Requisitante));

        sscanf(linha, "%d %[^\t\n] %10s %s",
               &novo_requisitante->id_requisitante,
               novo_requisitante->nome,
               novo_requisitante->data_nasc,
               novo_requisitante->id_freguesia);

        novo_requisitante->novo = 0; // Indica que foi carregado do arquivo
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
    }

    fclose(arquivo);
}

void imprimirRequisitantes() {
    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *atual = lista_requisitantes[i];
        while (atual != NULL) {
            printf("ID: %d\n", atual->id_requisitante);
            printf("Nome: %s\n", atual->nome);
            printf("Data de Nascimento: %s\n", atual->data_nasc);
            printf("ID Freguesia: %s\n", atual->id_freguesia);
            printf("Novo: %d\n", atual->novo);
            printf("-------------------------\n");
            atual = atual->prox;
        }
    }
}


int validarIdRequisitante(int id_requisitante) {
    int id = id_requisitante / 10;
    int controle = id_requisitante % 10;
    return controle == calcularAlgoritmoControle(id);
}
void listarUsuariosMaisRequisitantes() {
    FILE *arquivo = fopen("Requisitantes.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de requisitantes.\n");
        registarErro("Erro ao abrir o arquivo de requisitantes.\n");
        return;
    }

    int contadores[MAX_REQUISITANTES] = {0};
    char linha[256];
    int max_requisicoes = 0;

    // Ler o arquivo e contar as ocorrências de cada requisitante
    while (fgets(linha, sizeof(linha), arquivo)) {
        int id_requisitante;
        char nome[100], data_nasc[11], id_freguesia[10];
        sscanf(linha, "%d;%[^;];%[^;];%s", &id_requisitante, nome, data_nasc, id_freguesia);

        if (id_requisitante >= 0 && id_requisitante < MAX_REQUISITANTES) {
            contadores[id_requisitante]++;
            if (contadores[id_requisitante] > max_requisicoes) {
                max_requisicoes = contadores[id_requisitante];
            }
        }
    }

    fclose(arquivo);

    // Listar todos os requisitantes que têm o número máximo de ocorrências no arquivo
    printf("### USUÁRIOS MAIS PRESENTES NO ARQUIVO ###\n");
    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        if (contadores[i] == max_requisicoes && max_requisicoes > 0) {
            // Procurar o requisitante na lista de requisitantes
            Requisitante *temp = lista_requisitantes[i];
            while (temp != NULL) {
                if (temp->id_requisitante == i) {
                    printf("ID do requisitante: %d\n", temp->id_requisitante);
                    printf("Nome: %s\n", temp->nome);
                    printf("Número de vezes presente no arquivo: %d\n", contadores[i]);
                    printf("\n");
                    break;
                }
                temp = temp->prox;
            }
        }
    }
}

int hash(char *ISBN) {
    int soma = 0;
    for (int i = 0; i < strlen(ISBN); i++) {
        soma += ISBN[i];
    }
    return soma % MAX_LIVROS;
}

void adicionarLivroLista(Livro *livro) {
    // Calcula o índice na tabela hash baseado no ISBN
    int indice = hashISBN(livro->ISBN);

    // Insere o livro no início da lista ligada na tabela hash
    livro->prox = tabela_hash[indice];
    tabela_hash[indice] = livro;
}

// Função auxiliar para calcular o índice da tabela hash
int hashISBN(char *isbn) {
    // Aqui utilizamos uma simples soma dos valores dos caracteres do ISBN para o índice
    int soma = 0;
    for (int i = 0; isbn[i] != '\0'; i++) {
        soma += isbn[i];
    }
    return soma % MAX_LIVROS;
}
Livro* obterPrimeiroLivro() {
    for (int i = 0; i < MAX_LIVROS; i++) {
        if (tabela_hash[i] != NULL) {
            return tabela_hash[i];
        }
    }
    return NULL; // Retorna NULL se não encontrar nenhum livro
}
Livro* obterProximoLivro(Livro *livro) {
    if (livro->prox != NULL) {
        return livro->prox;
    }

    // Se não houver próximo na lista ligada, procura no próximo índice da tabela hash
    int indiceAtual = hashISBN(livro->ISBN);
    for (int i = indiceAtual + 1; i < MAX_LIVROS; i++) {
        if (tabela_hash[i] != NULL) {
            return tabela_hash[i];
        }
    }

    return NULL; // Retorna NULL se não houver próximo livro
}
//Logs

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


#endif //SIDEWORK_REQUICAO_H
