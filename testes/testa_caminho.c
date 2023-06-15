#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char *membros[] = {"/home/inf/xyz00/texto.doc", "membro.txt", NULL};

    char *diretorio_atual = getcwd(NULL, 0);  // Obtém o diretório atual
    int tamanho_diretorio_atual = strlen(diretorio_atual);

    for (int i = 0; membros[i] != NULL; i++) {
        if (membros[i][0] != '/') {
            // O membro não é um caminho absoluto, ignora a conversão
            continue;
        }

        char *caminho_relativo = malloc(strlen(membros[i]) + 2 + tamanho_diretorio_atual);
        if (caminho_relativo == NULL) {
            printf("Erro ao alocar memória.\n");
            return 1;
        }

        strcpy(caminho_relativo, ".");
        strcat(caminho_relativo, membros[i]);

        membros[i] = caminho_relativo;
    }

    free(diretorio_atual);

    // Exibindo os membros após a conversão
    for (int i = 0; membros[i] != NULL; i++) {
        printf("%s\n", membros[i]);
    }

    return 0;
}
