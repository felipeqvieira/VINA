#include "archiver.h"

void armazenar_caminho_relativo(Membro *membro) {
    char *nome = membro->nome;

    if (nome[0] != '/') {
        // O nome não é um caminho absoluto, não é necessário converter
        return;
    }

    char *diretorio_atual = getcwd(NULL, 0);
    int tamanho_diretorio_atual = strlen(diretorio_atual);

    char *caminho_relativo = malloc(strlen(nome) + 2 + tamanho_diretorio_atual);
    if (caminho_relativo == NULL) {
        printf("Erro ao alocar memória.\n");
        return;
    }

    strcpy(caminho_relativo, ".");
    strcat(caminho_relativo, nome + 1);  // Ignora a barra inicial do caminho absoluto

    strcpy(membro->nome, caminho_relativo);

    free(caminho_relativo);
}