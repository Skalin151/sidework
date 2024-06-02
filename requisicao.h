#ifndef SIDEWORK_REQUISICAO_H
#define SIDEWORK_REQUISICAO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <time.h>
#include "estruturas.h"
#include "hashing.h"


void carregarRequisitantes() {
    FILE *arquivo = fopen("Requisitantes.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de requisitantes.\n");
        return;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        Requisitante *novo_requisitante = (Requisitante *)malloc(sizeof(Requisitante));
        if (novo_requisitante == NULL) {
            printf("Erro: Falha ao alocar memória para o requisitante.\n");
            fclose(arquivo);
            return;
        }

        // Inicializa a estrutura para evitar dados incorretos
        memset(novo_requisitante, 0, sizeof(Requisitante));

        int id_requisitante;
        int qtd_requisitante;
        char nome[100], data_nasc[11], id_freguesia[10];
        int qtd_requisicoes;
        sscanf(linha, "%d %[^\t\n] %10s %s %d", &id_requisitante, nome, data_nasc, id_freguesia, &qtd_requisitante);

        novo_requisitante->id_requisitante = id_requisitante;
        strcpy(novo_requisitante->nome, nome);
        strcpy(novo_requisitante->data_nasc, data_nasc);
        strcpy(novo_requisitante->id_freguesia, id_freguesia);
        novo_requisitante->qtd_requisitante = qtd_requisitante;
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
            printf("Número de requisições: %d\n", atual->qtd_requisitante);
            printf("Novo: %d\n", atual->novo);
            printf("-------------------------\n");
            atual = atual->prox;
        }
    }
}


void listarUsuariosMaisRequisitantes() {
    // Variáveis para armazenar o ID do requisitante com o maior número de requisições
    int max_requisicoes = 0;
    int id_requisitante_max = -1;

    // Itera sobre os requisitantes para encontrar aquele com o maior número de requisições
    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *temp = lista_requisitantes[i];
        while (temp != NULL) {
            if (temp->qtd_requisitante > max_requisicoes) {
                max_requisicoes = temp->qtd_requisitante;
                id_requisitante_max = temp->id_requisitante;
            }
            temp = temp->prox;
        }
    }


void salvarRequisitantes() {
    FILE *arquivo = fopen("Requisitantes.txt", "w");
    if (!arquivo) {
        printf("Erro: Não foi possível abrir o arquivo Requisitantes.txt para escrita\n");
        return;
    }

    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *requisitante = lista_requisitantes[i];
        while (requisitante!= NULL) {
            fprintf(arquivo, "%d\t%s\t%s\t%s\t%d\n",
                    requisitante->id_requisitante, requisitante->nome, requisitante->data_nasc,
                    requisitante->id_freguesia, requisitante->qtd_requisitante);
            requisitante = requisitante->prox;
        }
    }

    fclose(arquivo);
    printf("Dados dos requisitantes salvos com sucesso!\n");
}


    // Se nenhum requisitante for encontrado, exibe uma mensagem e retorna
    if (id_requisitante_max == -1) {
        printf("Nenhum requisitante encontrado.\n");
        return;
    }

    // Imprime os detalhes do requisitante com o maior número de requisições
    printf("### USUÁRIO(S) COM MAIS REQUISIÇÕES ###\n");
    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *temp = lista_requisitantes[i];
        while (temp != NULL) {
            if (temp->id_requisitante == id_requisitante_max) {
                printf("ID do requisitante: %d\n", temp->id_requisitante);
                printf("Nome: %s\n", temp->nome);
                printf("Data de Nascimento: %s\n", temp->data_nasc);
                printf("ID Freguesia: %s\n", temp->id_freguesia);
                printf("Número de requisições: %d\n", max_requisicoes);
                printf("\n");
            }
            temp = temp->prox;
        }
    }
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

void gerarArquivoXML(const char *filename) {
    FILE *arquivo = fopen(filename, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    fprintf(arquivo, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(arquivo, "<biblioteca>\n");

    // Exportar livros
    fprintf(arquivo, "  <livros>\n");
    for (int i = 0; i < MAX_LIVROS; i++) {
        Livro *livro = tabela_hash[i];
        while (livro != NULL) {
            fprintf(arquivo, "    <livro>\n");
            fprintf(arquivo, "      <ISBN>%s</ISBN>\n", livro->ISBN);
            fprintf(arquivo, "      <titulo>%s</titulo>\n", livro->titulo);
            fprintf(arquivo, "      <autor>%s</autor>\n", livro->autor);
            fprintf(arquivo, "      <area>%s</area>\n", livro->area);
            fprintf(arquivo, "      <ano_publicacao>%d</ano_publicacao>\n", livro->ano_publicacao);
            fprintf(arquivo, "      <status>%d</status>\n", livro->status);
            fprintf(arquivo, "      <id_requisitante>%d</id_requisitante>\n", livro->id_requisitante);
            fprintf(arquivo, "      <qtd_requisicoes>%d</qtd_requisicoes>\n", livro->qtd_requisicoes);
            fprintf(arquivo, "    </livro>\n");
            livro = livro->prox;
        }
    }
    fprintf(arquivo, "  </livros>\n");

    // Exportar requisitantes
    fprintf(arquivo, "  <requisitantes>\n");
    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *req = lista_requisitantes[i];
        while (req != NULL) {
            fprintf(arquivo, "    <requisitante>\n");
            fprintf(arquivo, "      <id_requisitante>%d</id_requisitante>\n", req->id_requisitante);
            fprintf(arquivo, "      <nome>%s</nome>\n", req->nome);
            fprintf(arquivo, "      <data_nasc>%s</data_nasc>\n", req->data_nasc);
            fprintf(arquivo, "      <id_freguesia>%s</id_freguesia>\n", req->id_freguesia);
            fprintf(arquivo, "      <status>%d</status>\n", req->status);
            fprintf(arquivo, "      <novo>%d</novo>\n", req->novo);
            fprintf(arquivo, "    </requisitante>\n");
            req = req->prox;
        }
    }
    fprintf(arquivo, "  </requisitantes>\n");

    // Exportar requisições
    fprintf(arquivo, "  <requisicoes>\n");
    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        RequisicaoLivro *req = lista_requisicoes[i];
        while (req != NULL) {
            fprintf(arquivo, "    <requisicao>\n");
            fprintf(arquivo, "      <ISBN>%s</ISBN>\n", req->ISBN);
            fprintf(arquivo, "      <id_requisitante>%d</id_requisitante>\n", req->id_requisitante);
            fprintf(arquivo, "    </requisicao>\n");
            req = req->prox;
        }
    }
    fprintf(arquivo, "  </requisicoes>\n");

    fprintf(arquivo, "</biblioteca>\n");

    fclose(arquivo);
}

size_t calcularTamanhoTotal() {
    size_t tamanho_total = 0;

    tamanho_total += sizeof(Livro);
    tamanho_total += sizeof(Requisitante);
    tamanho_total += sizeof(RequisicaoLivro);
    tamanho_total += sizeof(Distrito) * MAX_DISTRITOS;
    tamanho_total += sizeof(Concelho) * MAX_CONCELHOS;
    tamanho_total += sizeof(Freguesia) * MAX_FREGUESIAS;
    tamanho_total += sizeof(AreaCount) * MAX_AREAS;

    return tamanho_total;
}

// Função para calcular a idade a partir da data de nascimento no formato "dd-mm-aaaa"
int calcularIdade(const char *data_nasc) {
    int dia, mes, ano;
    sscanf(data_nasc, "%2d-%2d-%4d", &dia, &mes, &ano);

    time_t t = time(NULL);
    struct tm *hoje = localtime(&t);

    int idade = hoje->tm_year + 1900 - ano;
    if (mes > (hoje->tm_mon + 1) || (mes == (hoje->tm_mon + 1) && dia > hoje->tm_mday)) {
        idade--;
    }

    return idade;
}

// Função para listar os requisitantes mais velhos
void listarRequisitantesMaisVelhos() {
    int idade_maxima = 0;

    // Primeiro, encontrar a idade máxima
    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *temp = lista_requisitantes[i];
        while (temp != NULL) {
            int idade = calcularIdade(temp->data_nasc);
            if (idade > idade_maxima) {
                idade_maxima = idade;
            }
            temp = temp->prox;
        }
    }

    // Segundo, listar os requisitantes que têm essa idade máxima
    printf("### REQUISITANTES MAIS VELHOS (IDADE: %d ANOS) ###\n", idade_maxima);
    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *temp = lista_requisitantes[i];
        while (temp != NULL) {
            int idade = calcularIdade(temp->data_nasc);
            if (idade == idade_maxima) {
                printf("ID: %d\n", temp->id_requisitante);
                printf("Nome: %s\n", temp->nome);
                printf("Data de Nascimento: %s\n", temp->data_nasc);
                printf("ID Freguesia: %s\n", temp->id_freguesia);
                printf("Novo: %d\n", temp->novo);
                printf("-------------------------\n");
            }
            temp = temp->prox;
        }
    }
}

float calcularMediaIdades() {
    int soma_idades = 0;
    int total_pessoas = 0;

    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *temp = lista_requisitantes[i];
        while (temp != NULL) {
            int idade = calcularIdade(temp->data_nasc);
            soma_idades += idade;
            total_pessoas++;
            temp = temp->prox;
        }
    }

    if (total_pessoas == 0) {
        printf("Não há pessoas na lista.\n");
        return 0;
    }

    float media_idades = (float)soma_idades / total_pessoas;
    printf("A média das idades de todas as pessoas é: %.2f anos.\n", media_idades);
    return media_idades;
}

int contarPessoasIdadeSuperior(int valor_limite) {
    int contador = 0;

    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *temp = lista_requisitantes[i];
        while (temp != NULL) {
            int idade = calcularIdade(temp->data_nasc);
            if (idade > valor_limite) {
                contador++;
            }
            temp = temp->prox;
        }
    }

    return contador;
}

int determinarIdadeMaisRequisitantes(int *idade_mais_requisitantes) {
    int contagem_idades[120] = {0}; // Assumindo uma idade máxima de 120 anos

    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *temp = lista_requisitantes[i];
        while (temp != NULL) {
            int idade = calcularIdade(temp->data_nasc);
            contagem_idades[idade]++;
            temp = temp->prox;
        }
    }

    int max_contagem = 0;
    int idade_mais_frequente = 0;

    for (int i = 0; i < 120; i++) {
        if (contagem_idades[i] > max_contagem) {
            max_contagem = contagem_idades[i];
            idade_mais_frequente = i;
        }
    }

    *idade_mais_requisitantes = idade_mais_frequente;
    return max_contagem;
}
void listarPessoasNuncaRequisitaramLivro(Requisitante *lista_requisitantes) {
    Requisitante *temp = lista_requisitantes;
    int flag = 0;

    while (temp != NULL) {
        if (temp->qtd_requisitante == 0) {
            flag = 1;
            printf("ID: %d\n", temp->id_requisitante);
            printf("Nome: %s\n", temp->nome);
            printf("Data de Nascimento: %s\n", temp->data_nasc);
            printf("ID Freguesia: %s\n", temp->id_freguesia);
            printf("Novo: %d\n", temp->novo);
            printf("-------------------------\n");
        }
        temp = temp->prox;
    }

    if (!flag) {
        printf("Não há pessoas que nunca requisitaram um livro.\n");
    }
}
#endif //SIDEWORK_REQUISICAO_H
