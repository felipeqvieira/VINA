#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOME_ARCHIVE 100
#define QUANTIDADE_MAXIMA 1000
#define MAX_NOME_ARCHIVE 100
#define MAX_NOME 100

typedef struct { 
    char nome[MAX_NOME];
    int user_ID;
    int permissoes;
    long tamanho;
    time_t data_modificacao;
    int ordem;
    long localizacao;
} Membro;

typedef struct {
    Membro membros[QUANTIDADE_MAXIMA];
    int num_membros;
    char nome_archive[MAX_NOME_ARCHIVE];
    Diretorio* diretorio;
} Archive;

typedef struct {
    Membro membros[QUANTIDADE_MAXIMA];
    int num_membros;
    char nome_archive[MAX_NOME_ARCHIVE];
} Diretorio;

typedef struct {
    Archive Archive;
} Archiver;
