#ifndef SIDEWORK_HASH_H
#define SIDEWORK_HASH_H
#include "estruturas.h"

// Função auxiliar para calcular o índice da tabela hash
int hashISBN(const char *isbn) {
    // Aqui utilizamos uma simples soma dos valores dos caracteres do ISBN para o índice
    int soma = 0;
    for (int i = 0; isbn[i] != '\0'; i++) {
        soma += isbn[i];
    }
    return soma % MAX_LIVROS;
}
int hash(const char *ISBN) {
    int soma = 0;
    for (int i = 0; i < strlen(ISBN); i++) {
        soma += ISBN[i];
    }
    return soma % MAX_LIVROS;
}
#endif //SIDEWORK_HASH_H
