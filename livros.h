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
#include "hashing.h"

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

// Função para verificar a existência de um requisitante pelo ID
int verificarExistenciaRequisitante(int id_requisitante) {
    int indice = id_requisitante % MAX_REQUISITANTES;
    Requisitante* temp = lista_requisitantes[indice];
    while (temp != NULL) {
        if (temp->id_requisitante == id_requisitante) {
            return 1;
        }
        temp = temp->prox;
    }
    return 0;
}

int verificarExistenciaRequisitantePorNome(const char* nome) {
    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante* temp = lista_requisitantes[i];
        while (temp != NULL) {
            if (strcmp(temp->nome, nome) == 0) {
                return 1;
            }
            temp = temp->prox;
        }
    }
    return 0;
}

// Função para gerar um novo ID de 9 dígitos
int gerarNovoID() {
    int id_requisitante;
    int soma, digito_controle;
    int id_ja_existe;

    srand(time(NULL)); // Inicializa a semente do gerador de números aleatórios

    do {
        soma = 0;
        id_requisitante = 10000000 + rand() % 90000000; // Gera um número de 8 dígitos (entre 10000000 e 99999999)
        for (int i = 0; i < 8; i++) {
            soma += (id_requisitante / (int)pow(10, i)) % 10;
        }
        digito_controle = (10 - (soma % 10)) % 10; // Calcula o dígito de controle
        id_requisitante = id_requisitante * 10 + digito_controle; // Adiciona o dígito de controle ao final

        id_ja_existe = verificarExistenciaRequisitante(id_requisitante);
    } while (id_ja_existe); // Repete até gerar um ID que não exista

    return id_requisitante;
}

void CriarRequisitante() {
    Requisitante* novo_requisitante = (Requisitante*)malloc(sizeof(Requisitante));
    if (novo_requisitante == NULL) {
        printf("Erro: Falha ao alocar memória para o novo requisitante.\n");
        return;
    }

    novo_requisitante->id_requisitante = gerarNovoID();

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
        Requisitante* temp = lista_requisitantes[indice];
        while (temp->prox != NULL) {
            temp = temp->prox;
        }
        temp->prox = novo_requisitante;
    }

    printf("Requisitante adicionado com sucesso. ID: %09d\n", novo_requisitante->id_requisitante);
}

// Função para validar a data de nascimento
int validarDataNascimento(char* data_nasc) {
    if (strlen(data_nasc) != 10) return 0;
    if (data_nasc[2] != '-' || data_nasc[5] != '-') return 0;
    for (int i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && !isdigit(data_nasc[i])) return 0;
    }
    return 1;
}

// Função para validar o ID da freguesia
int validarIdFreguesia(char* id_freguesia) {
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


void LivroMaisRequisitado() {
    int max_requisicoes = 0;
    Livro *livro_mais_requisitado = NULL;

    for (int i = 0; i < MAX_LIVROS; i++) {
        Livro *livro = tabela_hash[i];
        while (livro != NULL) {
            if (livro->qtd_requisicoes > max_requisicoes) {
                max_requisicoes = livro->qtd_requisicoes;
                livro_mais_requisitado = livro;
            }
            livro = livro->prox;
        }
    }

    if (livro_mais_requisitado != NULL) {
        printf("===== Livro Mais Requisitado =====\n");
        printf("ISBN: %s\n", livro_mais_requisitado->ISBN);
        printf("Titulo: %s\n", livro_mais_requisitado->titulo);
        printf("Autor: %s\n", livro_mais_requisitado->autor);
        printf("Area: %s\n", livro_mais_requisitado->area);
        printf("Ano de Publicacao: %d\n", livro_mais_requisitado->ano_publicacao);
        printf("Quantidade de Requisicoes: %d\n", livro_mais_requisitado->qtd_requisicoes);
        printf("================");
    }
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

void Areamaisrepetida() {
    AreaCount areaCounts[MAX_AREAS];
    int numAreas = 0;

    // Inicializar contadores de áreas
    for (int i = 0; i < MAX_AREAS; i++) {
        areaCounts[i].count = 0;
    }

    // Percorrer todos os livros na tabela hash
    for (int i = 0; i < MAX_LIVROS; i++) {
        Livro *livro = tabela_hash[i];
        while (livro != NULL) {
            // Verificar se a área já está na lista
            int areaIndex = -1;
            for (int j = 0; j < numAreas; j++) {
                if (strcmp(areaCounts[j].area, livro->area) == 0) {
                    areaIndex = j;
                    break;
                }
            }

            // Adicionar nova área ou incrementar contador de área existente
            if (areaIndex == -1) {
                if (numAreas < MAX_AREAS) {
                    strcpy(areaCounts[numAreas].area, livro->area);
                    areaCounts[numAreas].count = 1;
                    numAreas++;
                }
            } else {
                areaCounts[areaIndex].count++;
            }

            livro = livro->prox;
        }
    }

    // Encontrar o maior contador
    int maiorContador = 0;
    for (int i = 0; i < numAreas; i++) {
        if (areaCounts[i].count > maiorContador) {
            maiorContador = areaCounts[i].count;
        }
    }

    // Imprimir as áreas mais repetidas
    printf("As areas mais repetidas sao:\n");
    for (int i = 0; i < numAreas; i++) {
        if (areaCounts[i].count == maiorContador) {
            printf("%s (aparece %d vezes)\n", areaCounts[i].area, areaCounts[i].count);
        }
    }
}

void encontrarLivroMaisRecente() {
    Livro *livroMaisRecente = NULL;

    // Percorrer todos os livros na tabela hash
    for (int i = 0; i < MAX_LIVROS; i++) {
        Livro *livro = tabela_hash[i];
        while (livro != NULL) {
            if (livroMaisRecente == NULL || livro->ano_publicacao > livroMaisRecente->ano_publicacao) {
                livroMaisRecente = livro;
            }
            livro = livro->prox;
        }
    }

    // Exibir o livro mais recente
    if (livroMaisRecente != NULL) {
        printf("===== Livro com o Ano de Publicacao Mais Recente =====\n");
        printf("ISBN: %s\n", livroMaisRecente->ISBN);
        printf("Titulo: %s\n", livroMaisRecente->titulo);
        printf("Autor: %s\n", livroMaisRecente->autor);
        printf("Area: %s\n", livroMaisRecente->area);
        printf("Ano de Publicacao: %d\n", livroMaisRecente->ano_publicacao);
        printf("Quantidade de Requisições: %d\n", livroMaisRecente->qtd_requisicoes);
        printf("Status: %s\n", livroMaisRecente->status == REQUISITADO ? "Requisitado" : "Disponível");
        printf("ID do Requisitante: %d\n", livroMaisRecente->id_requisitante);
        printf("=====================================================\n");
    } else {
        printf("Nenhum livro encontrado na tabela.\n");
    }
}

void encontrarAreaMaisRequisitada() {
    AreaCount contadores[MAX_AREAS];
    int numAreas = 0;

    // Percorrer todos os livros na tabela hash
    for (int i = 0; i < MAX_LIVROS; i++) {
        Livro *livro = tabela_hash[i];
        while (livro != NULL) {
            // Verificar se a área já está no contador
            int found = 0;
            for (int j = 0; j < numAreas; j++) {
                if (strcmp(contadores[j].area, livro->area) == 0) {
                    contadores[j].count += livro->qtd_requisicoes;
                    found = 1;
                    break;
                }
            }
            // Se a área não estiver no contador, adicionar uma nova entrada
            if (!found && numAreas < MAX_AREAS) {
                strcpy(contadores[numAreas].area, livro->area);
                contadores[numAreas].count = livro->qtd_requisicoes;
                numAreas++;
            }
            livro = livro->prox;
        }
    }

    // Encontrar a área com o maior número de requisições
    AreaCount *areaMaisRequisitada = NULL;
    for (int i = 0; i < numAreas; i++) {
        if (areaMaisRequisitada == NULL || contadores[i].count > areaMaisRequisitada->count) {
            areaMaisRequisitada = &contadores[i];
        }
    }

    // Exibir a área mais requisitada
    if (areaMaisRequisitada != NULL) {
        printf("===== Area Mais Requisitada =====\n");
        printf("Area: %s\n", areaMaisRequisitada->area);
        printf("Quantidade de Requisicoes: %d\n", areaMaisRequisitada->count);
        printf("================================\n");
    } else {
        printf("Nenhuma área encontrada.\n");
    }
}

void adicionarLivroALista(Livro *livro) {
    for (int i = 0; i < numAreas; i++) {
        if (strcmp(areas[i].area, livro->area) == 0) {
            // Adicionar o livro à lista de livros desta área
            livro->prox = areas[i].livros;
            areas[i].livros = livro;
            areas[i].count++;
            return;
        }
    }

    // Se a área não estiver na lista, criar uma nova entrada para a área
    if (numAreas < MAX_AREAS) {
        strcpy(areas[numAreas].area, livro->area);
        areas[numAreas].livros = livro;
        areas[numAreas].count = 1;
        numAreas++;
    } else {
        printf("Erro: Numero maximo de areas atingido.\n");
    }
}

// Função para listar os livros requisitados organizados por área
void listarLivrosRequisitadosPorArea() {
    // Inicializar a lista de áreas
    for (int i = 0; i < MAX_AREAS; i++) {
        areas[i].livros = NULL;
        areas[i].count = 0;
    }
    numAreas = 0;

    // Percorrer todos os livros na tabela hash
    for (int i = 0; i < MAX_LIVROS; i++) {
        Livro *livro = tabela_hash[i];
        while (livro != NULL) {
            if (livro->status == 1) { // Livro requisitado
                adicionarLivroALista(livro);
            }
            livro = livro->prox;
        }
    }

    // Exibir os livros requisitados organizados por área
    printf("===== Livros Requisitados por Area =====\n");
    for (int i = 0; i < numAreas; i++) {
        printf("Área: %s\n", areas[i].area);
        Livro *livro = areas[i].livros;
        while (livro != NULL) {
            printf("  ISBN: %s\n", livro->ISBN);
            printf("  Titulo: %s\n", livro->titulo);
            printf("  Autor: %s\n", livro->autor);
            printf("  Ano de Publicacao: %d\n", livro->ano_publicacao);
            printf("  Requisitante ID: %d\n", livro->id_requisitante);
            printf("  Quantidade de Requisicoes: %d\n", livro->qtd_requisicoes);
            livro = livro->prox;
        }
        printf("\n");
    }
    printf("=======================================\n");
}

RequisicaoLivro *criarRequisicaoLivro(char *ISBN, int id_requisitante) {
    RequisicaoLivro *nova_requisicao = (RequisicaoLivro*)malloc(sizeof(RequisicaoLivro));
    if (nova_requisicao == NULL) {
        printf("Erro ao alocar memória para nova requisição de livro.\n");
        return NULL;
    }

    strcpy(nova_requisicao->ISBN, ISBN);
    nova_requisicao->id_requisitante = id_requisitante;
    nova_requisicao->prox = NULL;

    // Abrir o arquivo para escrita, utilizando modo "a" (append)
    FILE *arquivo = fopen("logs_requisicoes.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de logs.\n");
        free(nova_requisicao); // Liberar memória alocada se houver erro ao abrir o arquivo
        return NULL;
    }

    // Escrever os logs no arquivo
    fprintf(arquivo, "ISBN: %s, ID do Requisitante: %d\n", ISBN, id_requisitante);

    // Fechar o arquivo
    fclose(arquivo);

    return nova_requisicao;
}

void adicionarRequisicaoLivro(RequisicaoLivro **lista_requisicoes, RequisicaoLivro *nova_requisicao) {
    if (*lista_requisicoes == NULL) {
        *lista_requisicoes = nova_requisicao;
    } else {
        RequisicaoLivro *temp = *lista_requisicoes;
        while (temp->prox != NULL) {
            temp = temp->prox;
        }
        temp->prox = nova_requisicao;
    }
}

void efetuarRequisicaoLivro(char *ISBN, int id_requisitante) {
    int indice = hashISBN(ISBN);
    Livro *livro = tabela_hash[indice];

    // Encontra o requisitante correspondente
    Requisitante *requisitante = NULL;
    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *tmp = lista_requisitantes[i];
        while (tmp != NULL) {
            if (tmp->id_requisitante == id_requisitante) {
                requisitante = tmp;
                break;
            }
            tmp = tmp->prox;
        }
        if (requisitante != NULL) break;
    }

    if (requisitante == NULL) {
        printf("Requisitante não encontrado.\n");
        return;
    }

    // Percorre a lista ligada de livros para encontrar o livro com o ISBN correspondente
    while (livro != NULL) {
        if (strcmp(livro->ISBN, ISBN) == 0) {
            livro->status = REQUISITADO;
            livro->id_requisitante = id_requisitante;
            livro->qtd_requisicoes++;

            requisitante->qtd_requisitante++;
            printf("Livro requisitado com sucesso!\n");

            // Adiciona uma nova requisição de livro
            RequisicaoLivro *nova_requisicao = criarRequisicaoLivro(ISBN, id_requisitante);
            if (nova_requisicao != NULL) {
                adicionarRequisicaoLivro(&lista_requisicoes, nova_requisicao);
            } else {
                printf("Erro ao criar requisição de livro.\n");
            }
            return;
        }
        livro = livro->prox;
    }
    printf("Livro não encontrado.\n");
}

void consultarLogsRequisicoes() {
    // Abrir o arquivo para leitura
    FILE *arquivo = fopen("logs_requisicoes.txt", "r");
    if (arquivo == NULL) {
        printf("Nenhuma requisição de livro encontrada.\n");
        return;
    }

    printf("Logs de requisições de livro:\n");
    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        printf("%s", linha);
    }

    // Fechar o arquivo
    fclose(arquivo);
}

void LogsPessoais() {
    int id_requisitante;
    printf("Por favor, insira o ID do requisitante: ");
    scanf("%d", &id_requisitante);

    // Abrir o arquivo para leitura
    FILE *arquivo = fopen("logs_requisicoes.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de logs.\n");
        return;
    }

    char linha[256];
    int encontrou = 0;
    printf("ISBNs associados ao ID de requisitante %d:\n", id_requisitante);
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char ISBN[14];
        int id;
        // Ler os dados da linha
        if (sscanf(linha, "ISBN: %13s, ID do Requisitante: %d", ISBN, &id) == 2) {
            // Verificar se o ID corresponde ao ID fornecido
            if (id == id_requisitante) {
                printf("%s\n", ISBN);
                encontrou = 1;
            }
        }
    }

    if (!encontrou) {
        printf("Nenhum ISBN associado ao ID de requisitante %d encontrado.\n", id_requisitante);
    }

    // Fechar o arquivo
    fclose(arquivo);
}
#endif //SIDEWORK_LIVROS_H
