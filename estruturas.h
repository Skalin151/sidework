#ifndef ESTRUTURAS_H_INCLUDED
#define ESTRUTURAS_H_INCLUDED

#define MAX_LIVROS 100
#define MAX_REQUISITANTES 100
#define MAX_DISTRITOS 100
#define MAX_CONCELHOS 100
#define MAX_FREGUESIAS 100
#define MAX_REQ_LIVROS 100
#define REQUISITADO 1
#define NAO_REQUISITADO 0
#define MAX_LIVROS 100
#define MAX_AREAS 100

typedef struct Livro {
    char ISBN[14];
    char titulo[150];
    char autor[50];
    char area[50];
    int ano_publicacao;
    int status; // status do livro: 0 para nao requisitado, 1 para requisitado
    int id_requisitante;
    int qtd_requisicoes;
    struct Livro *prox; // Ponteiro para o proximo livro na lista ligada
} Livro;

typedef struct Requisitante {
    int id_requisitante;
    int status;
    char nome[100];
    char data_nasc[11]; // formato [dd-mm-aaaa]
    char id_freguesia[7]; // 6 caracteres (distrito + concelho + freguesia)
    int novo;
    int qtd_requisitante;
    struct Requisitante *prox; // Ponteiro para o proximo requisitante na lista ligada
} Requisitante;

typedef struct RequisicaoLivro {
    char ISBN[14];
    int id_requisitante;
    struct RequisicaoLivro *prox; // Ponteiro para a pr�xima requisi��o de livro na lista ligada
} RequisicaoLivro;

typedef struct Distrito {
    char id_distrito[3];
    char nome_distrito[50];
} Distrito;

typedef struct Concelho {
    char id_concelho[5];
    char nome_concelho[50];
} Concelho;

typedef struct Freguesia {
    char id_freguesia[7];
    char nome_freguesia[100];
} Freguesia;

typedef struct AreaCount {
    char area[50];
    int count;
    Livro *livros;
} AreaCount;


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

AreaCount areas[MAX_AREAS];
int numAreas = 0;

#endif // ESTRUTURAS_H_INCLUDED
