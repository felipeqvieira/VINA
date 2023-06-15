#include "archiver.h"
#include <stdio.h>
#include <stdlib.h>

// inicializa um archive que não existe.
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

// Verifica se o archive existe.
int arquivo_existe(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        return 0;  // Arquivo não existe
    }
    fclose(arquivo);
    return 1;  // Arquivo existe
}
