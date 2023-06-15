#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char nome[100];
    int user_ID;
    int permissoes;
    long tamanho;
    time_t data_modificacao;
    int ordem;
    long localizacao;
} Membro;

typedef struct {
    Membro membros[100];
    int num_membros;
    char nome_archive[50];
} Diretorio;

typedef struct {
    Membro membros[100];
    int num_membros;
    char nome_archive[50];
    Diretorio* diretorio;
} Archive;

typedef struct {
    Archive archive;
} Archiver;

void inicializar_archive(const char *nome_archive) {
    Archive archive;
    strcpy(archive.nome_archive, nome_archive);
    archive.num_membros = 0;
    archive.diretorio = (Diretorio*)malloc(sizeof(Diretorio));
    archive.diretorio->num_membros = 0;
    strcpy(archive.diretorio->nome_archive, nome_archive);

    FILE *arquivo = fopen(nome_archive, "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        free(archive.diretorio);
        return;
    }

    fwrite(&archive, sizeof(Archive), 1, arquivo);
    fclose(arquivo);

    printf("Archive '%s' criado com sucesso!\n", nome_archive);
}

void exibir_dados_archive(const char *nome_archive) {
    FILE *arquivo = fopen(nome_archive, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    Archive archive;
    fread(&archive, sizeof(Archive), 1, arquivo);
    fclose(arquivo);

    printf("Dados do Archive '%s':\n", nome_archive);
    printf("Nome: %s\n", archive.nome_archive);
    printf("Numero de membros: %d\n", archive.num_membros);
}

int arquivo_existe(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        return 0;  // Arquivo não existe
    }
    fclose(arquivo);
    return 1;  // Arquivo existe
}

int main() {
    const char *nome_archive = "meu_archive.vpp";

    if (arquivo_existe(nome_archive)){
        printf("O arquivo de archive ja existe.\n");
        
    } else{
        inicializar_archive(nome_archive);
    }

    exibir_dados_archive(nome_archive);
    
    return 0;
}
