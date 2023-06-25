#include "archiver.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// inicializa um archive que não existe.
void inicializar_archive(const char *nome_archive) {

    Archive archive;
    
    strcpy(archive.nome_archive, nome_archive);
    
    archive.diretorio.num_membros = 0;

    FILE *arquivo = fopen(nome_archive, "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    fwrite(&archive, sizeof(Archive), 1, arquivo);
    fclose(arquivo);

    printf("Archive '%s' criado com sucesso!\n", nome_archive);

}

// Verifica se o archive existe.
int arquivo_existe(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        return 0;  // Arquivo não existe
    }
    fclose(arquivo);
    return 1;  // Arquivo existe
}

int verifica_nome(Archive *archive, const char *nome_arquivo) {
    for (int i = 0; i < archive->diretorio.num_membros; i++) {
        if (strcmp(nome_arquivo, archive->diretorio.membros[i].nome) == 0) {
            return 1;  // O nome do arquivo já existe no Archive
        }
    }
    return 0;  // O nome do arquivo não existe no Archive
}

void extrair_informacoes_membro(const char *nome_arquivo, Membro *membro) {
    struct stat info_arquivo;

    // Verifica se o arquivo existe e obtém as informações
    if (stat(nome_arquivo, &info_arquivo) == -1) {
        perror("Erro ao obter informações do arquivo");
        return;
    }

    // Preenche as informações no membro
    strncpy(membro->nome, nome_arquivo, MAX_NOME - 1);
    membro->nome[MAX_NOME - 1] = '\0';

    // Remove os espaços do nome do arquivo
    const char *ptr = nome_arquivo;
    char *dest = membro->nome;
    while (*ptr) {
        if (*ptr != ' ') {
            *dest++ = *ptr;
        }
        ptr++;
    }
    *dest = '\0';

    membro->user_ID = info_arquivo.st_uid;
    membro->permissoes = info_arquivo.st_mode & 0777;
    membro->tamanho = info_arquivo.st_size;
    membro->data_modificacao = info_arquivo.st_mtime;
}