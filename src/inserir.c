#include "archiver.h"
#include "func_comp.h" 

/* insere um membro no archive */
void inserir_membros(Archiver *archiver, char **nomes_arquivos, int num_arquivos) {

    // Abre o arquivo do archive em modo de leitura e escrita binária
    FILE *arquivo_archive = fopen(archiver->nome_archive, "rb+");
    if (arquivo_archive == NULL) {
        printf("Erro ao abrir o arquivo do archive '%s'.\n", archiver->nome_archive);
        return;
    }

    // Move o cursor para o início do arquivo
    fseek(arquivo_archive, 0, SEEK_SET);

    // Insere os novos membros no início do arquivo
    for (int i = 0; i < num_arquivos; i++) {
        char *nome_membro = nomes_arquivos[i];

        // Verifica se o membro já existe no ArchiveData
        int membro_existente = 0;
        for (int j = 0; j < archiver->diretorio.num_membros; j++) {
            Membro *membro = &(archiver->diretorio.membros[j]);
            if (strcmp(membro->nome, nome_membro) == 0) {
                membro_existente = 1;
                break;
            }
        }
        if (membro_existente) {
            continue;  // Passa para o próximo arquivo
        }

        // Verifica se há espaço disponível no diretório
        if (archiver->diretorio.num_membros >= QUANTIDADE_MAXIMA) {
            printf("Não há espaço disponível para adicionar o membro '%s'.\n", nome_membro);
            return;
        }

        // Cria uma nova estrutura de Membro para armazenar as informações
        Membro novo_membro;
        extrair_informacoes_membro(archiver, nome_membro, &novo_membro);

        // Abre o arquivo do membro em modo de leitura binária
        FILE *arquivo_membro = fopen(nome_membro, "rb");
        if (arquivo_membro == NULL) {
            printf("Erro ao abrir o arquivo do membro '%s'.\n", nome_membro);
            continue;  // Passa para o próximo arquivo
        }

        long deslocamento = 0;
        for (int j = 0; j < archiver->diretorio.num_membros; j++) {
            deslocamento += archiver->diretorio.membros[j].tamanho;
        }

        novo_membro.localizacao = deslocamento;

        // Move o cursor para o início do conteúdo do membro
        fseek(arquivo_archive, deslocamento, SEEK_SET);

        // Insere o conteúdo do membro no arquivo
        while (1) {
            unsigned char buffer[1024];
            size_t bytes_lidos = fread(buffer, sizeof(unsigned char), 1024, arquivo_membro);
            if (bytes_lidos == 0) {
                break;  // Fim do arquivo
            }
            fwrite(buffer, sizeof(unsigned char), bytes_lidos, arquivo_archive);
        }

        // Fecha o arquivo do membro
        fclose(arquivo_membro);

        // Insere o novo membro no diretório
        archiver->diretorio.membros[archiver->diretorio.num_membros] = novo_membro;
        archiver->diretorio.num_membros++;
    }

    // Move o cursor para o início do diretório
    /* fseek(arquivo_archive, sizeof(ArchiveData), SEEK_SET);*/
    fseek(arquivo_archive, 0, SEEK_END);

    // Escreve o novo diretório no arquivo
    fwrite(&(archiver->diretorio), sizeof(Diretorio), 1, arquivo_archive);

    // Fecha o arquivo do archive
    fclose(arquivo_archive);
}