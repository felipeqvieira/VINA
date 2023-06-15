#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define MAX_NOME 100

typedef struct {
    char nome[MAX_NOME];
    int user_ID;
    int permissoes;
    long tamanho;
    time_t data_modificacao;
} Membro;

void preencher_dados_membro(Membro *membro, const char *nome_arquivo) {
    struct stat info_arquivo;

    if (stat(nome_arquivo, &info_arquivo) == -1) {
        perror("Erro ao obter informações do arquivo");
        return;
    }

    // Extrair informações do arquivo e preencher a estrutura Membro
    char nome_limpo[MAX_NOME];
    const char *nome_com_espacos = nome_arquivo;
    char *ponteiro_espaco;

    // Remover espaços extras no início do nome
    while (*nome_com_espacos == ' ') {
        nome_com_espacos++;
    }

    // Copiar o nome sem espaços extras
    strcpy(nome_limpo, nome_com_espacos);

    // Remover espaços extras no final do nome
    ponteiro_espaco = nome_limpo + strlen(nome_limpo) - 1;
    while (ponteiro_espaco > nome_limpo && *ponteiro_espaco == ' ') {
        *ponteiro_espaco = '\0';
        ponteiro_espaco--;
    }

    // Preencher a estrutura Membro
    strcpy(membro->nome, nome_limpo);
    membro->user_ID = info_arquivo.st_uid;
    membro->permissoes = info_arquivo.st_mode & 0777;
    membro->tamanho = info_arquivo.st_size;
    membro->data_modificacao = info_arquivo.st_mtime;
}

int main() {
    const char *nome_arquivo = "arquivo com espacos.txt";

    Membro membro;
    preencher_dados_membro(&membro, nome_arquivo);

    printf("Informações do arquivo:\n");
    printf("Nome: %s\n", membro.nome);
    printf("User ID: %d\n", membro.user_ID);
    printf("Permissões: %o\n", membro.permissoes);
    printf("Tamanho: %ld bytes\n", membro.tamanho);
    printf("Data de modificação: %s", ctime(&membro.data_modificacao));

    return 0;
}
