#ifndef SIDEWORK_REQUICAO_H
#define SIDEWORK_REQUICAO_H
#define MAX_LIVROS 100
#define MAX_REQUISITANTES 100
#define MAX_DISTRICTS 100
#define MAX_MUNICIPALITIES 100
#define MAX_PARISHES 100
#define MAX_REQ_LIVROS 100
#define REQUISITADO 1
#define NAO_REQUISITADO 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

// Adicione ou modifique a estrutura Livro para incluir o status
typedef struct Livro {
    char ISBN[14]; // ISBN (incluindo terminador de string)
    char titulo[100];
    char autor[100];
    char area[50];
    int ano_publicacao;
    int status; // status do livro: 0 para não requisitado, 1 para requisitado
    int id_requisitante;
    struct Livro *prox; // Ponteiro para o próximo livro na lista ligada
} Livro;

// Estrutura para representar um requisitante
typedef struct Requisitante {
    int id_requisitante;
    int status;
    char nome[100];
    char data_nasc[11]; // formato [dd-mm-aaaa]
    char id_freguesia[7]; // 6 caracteres (distrito + concelho + freguesia)
    int novo;
    struct Requisitante *prox; // Ponteiro para o próximo requisitante na lista ligada
} Requisitante;

// Estrutura para representar uma requisição de livro
typedef struct RequisicaoLivro {
    char ISBN[14];
    int id_requisitante;
    struct RequisicaoLivro *prox; // Ponteiro para a próxima requisição de livro na lista ligada
} RequisicaoLivro;

// Estrutura para representar um distrito
typedef struct District {
    char id_distrito[3];
    char nome_distrito[50];
} District;

// Estrutura para representar um concelho
typedef struct Municipality {
    char id_concelho[5];
    char nome_concelho[50];
} Municipality;

// Estrutura para representar uma freguesia
typedef struct Parish {
    char id_freguesia[7];
    char nome_freguesia[100];
} Parish;


// Tabela de hash para os livros
Livro *tabela_hash[MAX_LIVROS];

// Lista ligada para os requisitantes
Requisitante *lista_requisitantes[MAX_REQUISITANTES];
Requisitante *requisitantes_novos = NULL;

// Lista ligada para as requisições de livros
RequisicaoLivro *lista_requisicoes[MAX_REQ_LIVROS];

// Vetores para armazenar distritos, concelhos e freguesias
District districts[MAX_DISTRICTS];
Municipality municipalities[MAX_MUNICIPALITIES];
Parish parishes[MAX_PARISHES];


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

void listarRequisitantes() {
    printf("\n### LISTA DE NOVOS REQUISITANTES ###\n");
    for (int i = 0; i < MAX_REQUISITANTES; i++) {
        Requisitante *temp = lista_requisitantes[i];
        while (temp != NULL) {
            if (temp->novo) { // Lista apenas os novos requisitantes
                printf("ID: %d\n", temp->id_requisitante);
                printf("Nome: %s\n", temp->nome);
                printf("Data de nascimento: %s\n", temp->data_nasc);
                printf("ID Freguesia: %s\n", temp->id_freguesia);
                printf("\n");
            }
            temp = temp->prox;
        }
    }
}


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

/*void imprimirRequisitantes() {
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
}*/


int validarIdRequisitante(int id_requisitante) {
    int id = id_requisitante / 10;
    int controle = id_requisitante % 10;
    return controle == calcularAlgoritmoControle(id);
}
void listarUsuariosMaisRequisitantes() {
    FILE *arquivo = fopen("Requisitantes.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de requisitantes.\n");
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


#endif //SIDEWORK_REQUICAO_H
