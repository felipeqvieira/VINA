#include "archiver.h"

void remover_membros(Archiver *archiver, char *nome_archive, char **nomes_membros, int num_membros) {
    // Verifica se nenhum membro foi especificado para remoção
    int remover_todos_os_membros = (num_membros == 0);

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

    // Atualiza a estrutura do diretório removendo os membros desejados e reorganizando os índices
    int num_membros_atualizado = archiver->archiveData.diretorio.num_membros;
    int num_membros_removidos = 0;

    for (int i = 0; i < archiver->archiveData.diretorio.num_membros; i++) {
        Membro *membro = &(archiver->archiveData.diretorio.membros[i]);

        int membro_removido = 0;
        if (remover_todos_os_membros) {
            membro_removido = 1;
            num_membros_atualizado--;
            num_membros_removidos++;
        } else {
            for (int j = 0; j < num_membros; j++) {
                if (strcmp(membro->nome, nomes_membros[j]) == 0) {
                    membro_removido = 1;
                    num_membros_atualizado--;
                    num_membros_removidos++;
                    break;
                }
            }
        }

        if (!membro_removido) {
            if (num_membros_removidos > 0) {
                // Move o membro para a posição atualizada
                archiver->archiveData.diretorio.membros[i - num_membros_removidos] = *membro;
            }
        }
    }

    archiver->archiveData.diretorio.num_membros = num_membros_atualizado;

    // Passa os membros do diretório em ordem, escrevendo no arquivo temporário os membros que não foram escolhidos para serem removidos
    for (int i = 0; i < num_membros_atualizado; i++) {
        Membro *membro = &(archiver->archiveData.diretorio.membros[i]);

        long deslocamento = 0;
        for (int j = 0; j < i; j++) {
            deslocamento += archiver->archiveData.diretorio.membros[j].tamanho;
        }

        fseek(arquivo_original, deslocamento, SEEK_SET);

        long tamanho_membro = membro->tamanho;

        unsigned char buffer[MAX_CONTEUDO];

        int quociente = tamanho_membro / MAX_CONTEUDO;

        for (int j = 0; j < quociente; j++) {
            size_t bytes_lidos = fread(buffer, sizeof(unsigned char), MAX_CONTEUDO, arquivo_original);

            printf("conteudo membro: %s\n", buffer);

            if (bytes_lidos == 0) {
                printf("Erro ao ler o conteúdo do archive.\n");
                fclose(arquivo_temporario);
                remove("temp_archive");
                break;
            }

            size_t bytes_gravados = fwrite(buffer, sizeof(unsigned char), bytes_lidos, arquivo_temporario);

            if (bytes_gravados < bytes_lidos) {
                printf("Erro ao gravar o conteúdo no arquivo extraído.\n");
                fclose(arquivo_temporario);
                remove("temp_archive");
                break;
            }
        }

        int resto = tamanho_membro % MAX_CONTEUDO;

        if (resto > 0) {
            size_t bytes_lidos = fread(buffer, sizeof(unsigned char), resto, arquivo_original);

            printf("conteudo membro: %s\n", buffer);

            if (bytes_lidos == 0) {
                printf("Erro ao ler o conteúdo do arquivo.\n");
                fclose(arquivo_temporario);
                remove("temp_archive");
                continue;
            }

            size_t bytes_gravados = fwrite(buffer, sizeof(unsigned char), bytes_lidos, arquivo_temporario);

            if (bytes_gravados < bytes_lidos) {
                printf("Erro ao gravar o conteúdo no arquivo extraído.\n");
                fclose(arquivo_temporario);
                remove("temp_archive");
                continue;
            }
        }
    }

    fseek(arquivo_temporario, 0, SEEK_END);

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

    // Renomeia o arquivo temporário para substituir o arquivo original do archive
    if (remove(nome_archive) != 0) {
        printf("Erro ao remover o arquivo original do archive.\n");
        remove("temp_archive");
        return;
    }

    if (rename("temp_archive", nome_archive) != 0) {
        printf("Erro ao renomear o arquivo temporário para substituir o arquivo original do archive.\n");
        remove("temp_archive");
        return;
    }
    
}