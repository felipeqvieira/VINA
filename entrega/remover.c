#include "archiver.h"

void remover_membros(Archiver *archiver, char *nome_archive, char **nomes_membros, int num_membros) {
    // Abre o arquivo original do archive para leitura
    FILE *arquivo_original = fopen(nome_archive, "rb");
    if (arquivo_original == NULL) {
        printf("Erro ao abrir o arquivo original do archive.\n");
        return;
    }

    // Cria o arquivo temporário do archive para escrita
    FILE *arquivo_temporario = fopen("temp_archive", "wb");
    if (arquivo_temporario == NULL) {
        printf("Erro ao criar o arquivo temporário do archive.\n");
        fclose(arquivo_original);
        return;
    }

    // Passa os membros do diretório em ordem, escrevendo no arquivo temporário os membros que não foram escolhidos para serem removidos
    for (int i = 0; i < archiver->archiveData.diretorio.num_membros; i++) {

        printf("Numero de membros: %d\n", archiver->archiveData.diretorio.num_membros);

        Membro *membro = &(archiver->archiveData.diretorio.membros[i]);

        int membro_removido = 0;
        for (int j = 0; j < num_membros; j++) {
            if (strcmp(membro->nome, nomes_membros[j]) == 0) {
                membro_removido = 1;
                break;
            }
        }

        if (!membro_removido) {
            
        }

    }

    // Escreve o diretório atualizado no final do arquivo temporário do archive
    if (fwrite(&(archiver->archiveData.diretorio), sizeof(Diretorio), 1, arquivo_temporario) != 1) {
        printf("Erro ao escrever o diretório atualizado no arquivo temporário do archive.\n");
        fclose(arquivo_original);
        fclose(arquivo_temporario);
        remove("temp_archive");
        return;
    }

    // Fecha os arquivos
    fclose(arquivo_original);
    fclose(arquivo_temporario);

    /*

    // Renomeia o arquivo temporário para substituir o arquivo original do archive
    if (rename("temp_archive", nome_archive) != 0) {
        printf("Erro ao renomear o arquivo temporário para substituir o arquivo original do archive.\n");
        remove("temp_archive");
        return;
    }
    */

    printf("Membros removidos com sucesso do archive.\n");
}
