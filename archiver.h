#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOME_ARCHIVE 100
#define QUANTIDADE_MAXIMA 1000
#define MAX_NOME_ARCHIVE 100
#define MAX_NOME 100
#define MAX_CONTEUDO 1000000
#define BUFFER_SIZE 1024

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
} Diretorio;

typedef struct {
    unsigned char conteudo[MAX_CONTEUDO];
} ConteudoMembro;


typedef struct {
    Diretorio diretorio;
    ConteudoMembro conteudo;
    char nome_archive[MAX_NOME_ARCHIVE];
} Archive;

typedef struct {
    Archive Archive;
} Archiver;
