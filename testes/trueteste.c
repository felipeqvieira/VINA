#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define MAX_NOME 100
#define MAX_NOME_ARCHIVE 50
#define MAX_CONTEUDO 1024
#define QUANTIDADE_MAXIMA 100

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
} Diretorio;

typedef struct {
    unsigned char conteudo[MAX_CONTEUDO];
} ConteudoMembro;

typedef struct {
    Diretorio diretorio;
    ConteudoMembro conteudo;
} ArchiveData;

typedef struct {
    ArchiveData archiveData;
} Archiver;

void preencher_dados_membro(Membro *membro, const char *nome_arquivo) {
    struct stat file_info;

    if (stat(nome_arquivo, &file_info) == -1) {
        perror("Erro ao obter informações do arquivo");
        return;
    }

    strcpy(membro->nome, nome_arquivo);
    membro->tamanho = file_info.st_size;
    membro->data_modificacao = file_info.st_mtime;
    membro->user_ID = file_info.st_uid;
    membro->permissoes = (file_info.st_mode & S_IRWXU) | 0070 | 0007;
    // Preencha as demais propriedades do membro, se necessário
}

void inicializar_archive(const char *nome_archive) {
    ArchiveData archiveData;
    strcpy(archiveData.diretorio.nome_archive, nome_archive);
    archiveData.diretorio.num_membros = 0;

    FILE *arquivo = fopen(nome_archive, "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fwrite(&archiveData, sizeof(ArchiveData), 1, arquivo);
    fclose(arquivo);

    printf("Archive '%s' criado com sucesso!\n", nome_archive);
}

int arquivo_existe(const char *nome_archive) {
    FILE *arquivo = fopen(nome_archive, "rb");
    if (arquivo == NULL) {
        return 0;
    }
    fclose(arquivo);
    return 1;
}

void exibir_dados_membro(const Membro *membro) {
    printf("Nome: %s\n", membro->nome);
    printf("User ID: %d\n", membro->user_ID);
    printf("Permissões: %d\n", membro->permissoes);
    printf("Tamanho: %ld\n", membro->tamanho);
    printf("Data de modificação: %ld\n", membro->data_modificacao);
    printf("Ordem: %d\n", membro->ordem);
    printf("Localização: %ld\n", membro->localizacao);
}

void exibir_dados_archive(const char *nome_archive) {
    FILE *arquivo = fopen(nome_archive, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    ArchiveData archiveData;
    fread(&archiveData, sizeof(ArchiveData), 1, arquivo);
    fclose(arquivo);

    printf("Dados do Archive '%s':\n", nome_archive);
    printf("Nome: %s\n", archiveData.diretorio.nome_archive);
    printf("Número de membros: %d\n", archiveData.diretorio.num_membros);

    // Exibir os dados de cada membro
    for (int i = 0; i < archiveData.diretorio.num_membros; i++) {
        Membro *membro = &archiveData.diretorio.membros[i];
        exibir_dados_membro(membro);
    }
}

int main() {
    const char *nome_archive = "meu_archive.vpp";

    if (!arquivo_existe(nome_archive)) {
        inicializar_archive(nome_archive);
    }

    // Teste de inserção de membro e exibição dos dados
    Archiver archiver;
    ArchiveData *archiveData = &archiver.archiveData;
    Diretorio *diretorio = &archiveData->diretorio;
    ConteudoMembro *conteudo = &archiveData->conteudo;

    strcpy(diretorio->membros[0].nome, "arquivo.txt");
    preencher_dados_membro(&diretorio->membros[0], "arquivo.txt");
    diretorio->membros[0].ordem = 1; // Define a ordem do primeiro membro como 1
    
    // Copiar o conteúdo do arquivo para archiver.archiveData.conteudo
    FILE *arquivo = fopen("arquivo.txt", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    fseek(arquivo, 0, SEEK_END);
    long tamanho_arquivo = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);

    if (tamanho_arquivo > MAX_CONTEUDO) {
        printf("Tamanho do arquivo excede o limite.\n");
        fclose(arquivo);
        return 1;
    }

    fread(conteudo, tamanho_arquivo, 1, arquivo);
    fclose(arquivo);

    // Atualizar os dados do diretório com as propriedades do membro
    diretorio->num_membros++;

    // Salvar archiver.archiveData no arquivo
    arquivo = fopen(nome_archive, "rb+");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    fwrite(&archiver, sizeof(Archiver), 1, arquivo);
    fclose(arquivo);

    exibir_dados_archive(nome_archive);

    return 0;
}
