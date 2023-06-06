#include <sys/stat.h>
#include <stdio.h>

#define ARQUIVO_VERIFICADO 0 // constante caso sucesso
#define ERRO_NA_ABERTURA -1
#define ARQUIVO_NAO_ENCONTRADO -2
#define ERRO_FSEEK -3
#define ERRO_FTELL -4
#define QUANTIDADE_MAXIMA 1000 // quantidade maxima de arquivos no diretorio
#define BUFFER_SIZE 1024

typedef struct { 

    char nome[100];
    int user_ID;
    int permissoes;
    long tamanho;
    time_t data_modificacao;
    int ordem;
    long localizacao;

}Membro;

typedef struct {
    Membro* membros[QUANTIDADE_MAXIMA];
    int num_membros;
} Archive;

typedef struct {

    Archive archive;

}Archiver;