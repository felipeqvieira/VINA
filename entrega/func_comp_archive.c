#include "archiver.h"

void verificar_archive_existente(Archiver *archiver, const char *nome_arquivo) {

    FILE *arquivo = fopen(nome_arquivo, "r+b");
    if (arquivo == NULL) {
        printf("O arquivo '%s' nao existe. Um novo arquivo sera criado.\n", nome_arquivo);

        // Cria um novo arquivo vazio
        arquivo = fopen(nome_arquivo, "w+b");
        if (arquivo == NULL) {
            printf("Erro ao criar o arquivo '%s'.\n", nome_arquivo);
            return;
        }

        archiver->archiveData.diretorio.num_membros = 0;

        // Inicializar o diretório alocando memória
        memset(archiver->archiveData.diretorio.membros, 0, sizeof(archiver->archiveData.diretorio.membros));

        // Salva a estrutura do diretório no arquivo
        fwrite(&(archiver->archiveData.diretorio), sizeof(Diretorio), 1, arquivo);

        fclose(arquivo);

        return;
    }

    fseek(arquivo, -sizeof(Diretorio), SEEK_END);
    fread(&(archiver->archiveData.diretorio), sizeof(Diretorio), 1, arquivo);
    fclose(arquivo);

    printf("Archive '%s' encontrado. Diretorio carregado com sucesso.\n", nome_arquivo);
}