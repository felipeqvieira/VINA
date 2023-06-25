#include "archiver.h"

/* verifica se o membro existe no archive */
int verifica_nome(Archiver *archive, const char *nome_arquivo) {
    for (int i = 0; i < archive->archiveData.diretorio.num_membros; i++) {
        if (strcmp(nome_arquivo, archive->archiveData.diretorio.membros[i].nome) == 0) {
            return i;  // O nome do arquivo já existe no Archive
        }
    }
    return -1;  // O nome do arquivo não existe no Archive
}

int contar_membros(char **membros) {
    int contador = 0;
    while (membros[contador] != NULL) {
        contador++;
    }
    return contador;
}


void extrair_informacoes_membro(Archiver *archiver, const char *nome_arquivo, Membro *membro) {
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

    armazenar_caminho_relativo(membro);

    membro->user_ID = info_arquivo.st_uid;
    membro->permissoes = info_arquivo.st_mode & 0777;
    membro->tamanho = info_arquivo.st_size;
    membro->data_modificacao = info_arquivo.st_mtime;
    membro->ordem = archiver->archiveData.diretorio.num_membros; 
    membro->localizacao = archiver->archiveData.diretorio.num_membros; 
}