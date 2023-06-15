#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME_ARCHIVE 100
#define QUANTIDADE_MAXIMA 1000
#define MAX_NOME 100
#define MAX_CONTEUDO 1000000

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
} ArchiveData;

typedef struct {
    ArchiveData archiveData;
} Archiver;

// Verifica se o archive existe.
int arquivo_existe(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        return 0;  // Arquivo não existe
    }
    fclose(arquivo);
    return 1;  // Arquivo existe
}

ArchiveData* inicializar_archive(const char *nome_archive) {

    if (arquivo_existe(nome_archive) != 0) {
        printf("O arquivo '%s' já existe.\n", nome_archive);
        return NULL;
    }

    ArchiveData* archiveData = malloc(sizeof(ArchiveData));

    if (archiveData == NULL) {
        printf("Erro ao alocar memória para o ArchiveData.\n");
        return NULL;
    }
    
    strcpy(archiveData->nome_archive, nome_archive);
    archiveData->diretorio.num_membros = 0;

    FILE *arquivo = fopen(nome_archive, "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        free(archiveData);
        return NULL;
    }

    fwrite(archiveData, sizeof(ArchiveData), 1, arquivo);
    fclose(arquivo);

    printf("ArchiveData '%s' criado com sucesso!\n", nome_archive);

    return archiveData;
}


int main(){

    Archiver *archiver = malloc(sizeof(Archiver));

    archiver->archiveData = *inicializar_archive("felipe.vpp");
    
    return 1;
}