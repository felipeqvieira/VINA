#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOME_ARQUIVO 100
#define MAX_NOME_ARCHIVE 1024
#define QUANTIDADE_MAXIMA 1000

typedef struct {
    char nome[MAX_NOME_ARQUIVO];
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
} Archiver;

void adicionar_membro(Archiver* archiver, const char* nome_membro) {
    if (archiver->num_membros >= QUANTIDADE_MAXIMA) {
        printf("Quantidade máxima de membros atingida.\n");
        return;
    }

    Membro novo_membro;
    strncpy(novo_membro.nome, nome_membro, MAX_NOME_ARQUIVO);
    novo_membro.user_ID = 1;
    novo_membro.permissoes = 755;
    novo_membro.tamanho = 1000;
    novo_membro.data_modificacao = time(NULL);
    novo_membro.ordem = archiver->num_membros;
    novo_membro.localizacao = 0;

    archiver->membros[archiver->num_membros] = novo_membro;
    archiver->num_membros++;
}

void listar_conteudo(Archiver* archiver) {
    printf("Conteúdo do arquivo '%s':\n", archiver->nome_archive);

    for (int i = 0; i < archiver->num_membros; i++) {
        Membro membro = archiver->membros[i];
        printf("Nome: %s\n", membro.nome);
        printf("ID do Usuário: %d\n", membro.user_ID);
        printf("Permissões: %d\n", membro.permissoes);
        printf("Tamanho: %ld\n", membro.tamanho);
        printf("Data de Modificação: %s", ctime(&membro.data_modificacao));
        printf("Ordem: %d\n", membro.ordem);
        printf("Localização: %ld\n", membro.localizacao);
        printf("----------------------\n");
    }
}

int main() {
    Archiver archiver;
    const char* nome_archive = "arquivo_archive.bin";

    strncpy(archiver.nome_archive, nome_archive, MAX_NOME_ARCHIVE);
    archiver.num_membros = 0;

    adicionar_membro(&archiver, "membro1.txt");
    adicionar_membro(&archiver, "membro2.txt");
    adicionar_membro(&archiver, "membro3.txt");

    listar_conteudo(&archiver);

    return 0;
}
