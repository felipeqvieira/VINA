#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define QUANTIDADE_MAXIMA 100

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
    Membro* membros[QUANTIDADE_MAXIMA];
    int num_membros;
} Archive;

typedef struct {
    Archive archive;
} Archiver;

void inicializar_membro(Membro* membro) {
    memset(membro->nome, 0, sizeof(membro->nome));
    membro->user_ID = 0;
    membro->permissoes = 0;
    membro->tamanho = 0;
    membro->data_modificacao = 0;
    membro->ordem = 0;
    membro->localizacao = 0;
}

void inicializar_archive(Archive* archive) {
    int i;
    for (i = 0; i < QUANTIDADE_MAXIMA; i++) {
        archive->membros[i] = NULL;
    }
    archive->num_membros = 0;
}

void preencher_dados_membro(Membro* membro, const char* nome, int user_ID, int permissoes, long tamanho, time_t data_modificacao, int ordem, long localizacao) {
    strcpy(membro->nome, nome);
    membro->user_ID = user_ID;
    membro->permissoes = permissoes;
    membro->tamanho = tamanho;
    membro->data_modificacao = data_modificacao;
    membro->ordem = ordem;
    membro->localizacao = localizacao;
}

void adicionar_membro(Archiver* archiver, Membro* membro) {
    if (archiver->archive.num_membros < QUANTIDADE_MAXIMA) {
        archiver->archive.membros[archiver->archive.num_membros] = membro;
        archiver->archive.num_membros++;
    }
}

void substituir_membro(Archiver* archiver, int index, Membro* novo_membro) {
    if (index >= 0 && index < archiver->archive.num_membros) {
        Membro* membro_antigo = archiver->archive.membros[index];
        free(membro_antigo);
        archiver->archive.membros[index] = novo_membro;
    }
}

int encontrar_nome(Archiver* archiver, const char* nome) {
    int i;
    for (i = 0; i < archiver->archive.num_membros; i++) {
        Membro* membro = archiver->archive.membros[i];
        if (strcmp(membro->nome, nome) == 0) {
            return i;
        }
    }
    return -1;
}

void adiciona_verificando_data(Archiver* archiver, Membro* membro) {
    int index = encontrar_nome(archiver, membro->nome);
    if (index != -1) {
        Membro* membro_existente = archiver->archive.membros[index];
        if (membro->data_modificacao > membro_existente->data_modificacao) {
            substituir_membro(archiver, index, membro);
        }
    } else {
        adicionar_membro(archiver, membro);
    }
}

void atualizar_diretorio(Archiver* archiver) {
    int i;
    for (i = 0; i < archiver->archive.num_membros; i++) {
        Membro* membro = archiver->archive.membros[i];
        membro->ordem = i;
    }
}

void escrever_arquivo_archive(Archiver* archiver, const char* nome_arquivo) {
    FILE* arquivo = fopen(nome_arquivo, "wb");
    if (arquivo != NULL) {
        fwrite(&archiver->archive, sizeof(Archive), 1, arquivo);
        fclose(arquivo);
    }
}

void mover_membro(Archiver* archiver, int indice_membro, int indice_destino) {
    if (indice_membro >= 0 && indice_membro < archiver->archive.num_membros &&
        indice_destino >= 0 && indice_destino < archiver->archive.num_membros) {
        Membro* membro = archiver->archive.membros[indice_membro];
        int i;
        if (indice_membro < indice_destino) {
            for (i = indice_membro; i < indice_destino; i++) {
                archiver->archive.membros[i] = archiver->archive.membros[i + 1];
            }
        } else {
            for (i = indice_membro; i > indice_destino; i--) {
                archiver->archive.membros[i] = archiver->archive.membros[i - 1];
            }
        }
        archiver->archive.membros[indice_destino] = membro;
        atualizar_diretorio(archiver);
    }
}

void extrair_membros(Archiver* archiver, const char* nome_arquivo) {
    FILE* arquivo = fopen(nome_arquivo, "wb");
    if (arquivo != NULL) {
        int i;
        for (i = 0; i < archiver->archive.num_membros; i++) {
            Membro* membro = archiver->archive.membros[i];
            FILE* membro_arquivo = fopen(membro->nome, "wb");
            if (membro_arquivo != NULL) {
                fwrite(membro, sizeof(Membro), 1, membro_arquivo);
                fclose(membro_arquivo);
            }
        }
        fclose(arquivo);
    }
}

int main() {
    // Inicializar o archiver
    Archiver archiver;
    inicializar_archive(&archiver.archive);

    // Adicionar membros de exemplo
    Membro* membro1 = (Membro*)malloc(sizeof(Membro));
    preencher_dados_membro(membro1, "arquivo1.txt", 1, 644, 1024, time(NULL), 0, 0);
    adicionar_membro(&archiver, membro1);

    Membro* membro2 = (Membro*)malloc(sizeof(Membro));
    preencher_dados_membro(membro2, "arquivo2.txt", 2, 755, 2048, time(NULL), 1, 1024);
    adicionar_membro(&archiver, membro2);

    // Testar as funções
    printf("Conteúdo do Archive:\n");
    int i;
    for (i = 0; i < archiver.archive.num_membros; i++) {
        Membro* membro = archiver.archive.membros[i];
        printf("Nome: %s\n", membro->nome);
        printf("UID: %d\n", membro->user_ID);
        printf("Permissões: %d\n", membro->permissoes);
        printf("Tamanho: %ld\n", membro->tamanho);
        printf("Data de modificação: %ld\n", membro->data_modificacao);
        printf("Ordem: %d\n", membro->ordem);
        printf("Localização: %ld\n\n", membro->localizacao);
    }

    printf("Movendo membro...\n");
    mover_membro(&archiver, 1, 0);

    printf("Novo conteúdo do Archive:\n");
    for (i = 0; i < archiver.archive.num_membros; i++) {
        Membro* membro = archiver.archive.membros[i];
        printf("Nome: %s\n", membro->nome);
        printf("UID: %d\n", membro->user_ID);
        printf("Permissões: %d\n", membro->permissoes);
        printf("Tamanho: %ld\n", membro->tamanho);
        printf("Data de modificação: %ld\n", membro->data_modificacao);
        printf("Ordem: %d\n", membro->ordem);
        printf("Localização: %ld\n\n", membro->localizacao);
    }

    printf("Extraindo membros...\n");
    extrair_membros(&archiver, "arquivo_archive");

    return 0;
}
