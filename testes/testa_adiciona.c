#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME_ARQUIVO 100
#define MAX_NOME_ARCHIVE 1024
#define QUANTIDADE_MAXIMA 1000

typedef struct {
    char nome[MAX_NOME_ARQUIVO];
} Archive;

typedef struct {
    Archive membros[QUANTIDADE_MAXIMA];
    int num_membros;
    char nome_archive[MAX_NOME_ARCHIVE];
} Archiver;

void adicionar_membro(Archiver* archiver, const char* nome_membro) {
    if (archiver->num_membros >= QUANTIDADE_MAXIMA) {
        printf("Quantidade máxima de membros atingida.\n");
        return;
    }

    strncpy(archiver->membros[archiver->num_membros].nome, nome_membro, MAX_NOME_ARQUIVO);
    archiver->num_membros++;
}

void listar_conteudo(Archiver* archiver) {
    printf("Conteúdo do arquivo '%s':\n", archiver->nome_archive);

    for (int i = 0; i < archiver->num_membros; i++) {
        printf("Nome: %s\n", archiver->membros[i].nome);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso incorreto. Informe o nome do arquivo archive e pelo menos um membro.\n");
        return 1;
    }

    Archiver archiver;
    const char* nome_archive = argv[1];

    // Definir o nome do arquivo archive
    strncpy(archiver.nome_archive, nome_archive, MAX_NOME_ARCHIVE);
    archiver.num_membros = 0;

    // Adicionar membros
    for (int i = 2; i < argc; i++) {
        adicionar_membro(&archiver, argv[i]);
    }

    // Listar conteúdo do arquivo archive
    listar_conteudo(&archiver);

    return 0;
}
