#include "archiver.h"
#include "func_comp.h"

/* Extrai um membro do archive */
void extrair_membro(Archiver *archiver, char *nome_archive, char **nomes_membros, int nume_membros) {

    // Abre o arquivo do archive em modo de leitura binária
    FILE *arquivo_archive = fopen(nome_archive, "rb");
    if (arquivo_archive == NULL) {
        printf("Erro ao abrir o arquivo do archive '%s'.\n", nome_archive);
        return;
    }

    // Obtém o tamanho do arquivo sem considerar o diretório
    fseek(arquivo_archive, 0, SEEK_END);
    long tamanho_arquivo = ftell(arquivo_archive);

    fseek(arquivo_archive, 0, SEEK_SET);
    tamanho_arquivo -= sizeof(ArchiveData);

    // Verifica se todos os membros devem ser extraídos
    int extrair_todos = (nume_membros == 0);

    // Percorre a lista de membros e extrai cada um
    for (int i = 0; i < archiver->archiveData.diretorio.num_membros; i++) {
        Membro *membro = &(archiver->archiveData.diretorio.membros[i]);
        const char *nome_membro = membro->nome;

        // Verifica se o membro deve ser extraído
        int extrair_membro = extrair_todos;
        for (int j = 0; j < nume_membros; j++) {
            if (strcmp(nome_membro, nomes_membros[j]) == 0) {
                extrair_membro = 1;
                break;
            }
        }

        if (!extrair_membro) {
            continue; // Pula para o próximo membro a ser extraído
        }

        // Verifica se o diretório do membro existe e cria-o, se necessário
        char *diretorio_membro = strdup(nome_membro);
        char *ultimo_slash = strrchr(diretorio_membro, '/');
        if (ultimo_slash != NULL) {
            *ultimo_slash = '\0';
            criar_diretorio(diretorio_membro);
        }
        free(diretorio_membro);

        // Calcula o deslocamento necessário para chegar ao início do conteúdo do membro
        long deslocamento = 0;
        for (int j = 0; j < i; j++) {
            deslocamento += archiver->archiveData.diretorio.membros[j].tamanho;
        }

        // Move o ponteiro para o início do conteúdo do membro
        fseek(arquivo_archive, deslocamento, SEEK_SET);

        // Cria o arquivo do membro para escrita
        FILE *arquivo_membro = fopen(nome_membro, "wb");
        if (arquivo_membro == NULL) {
            printf("Erro ao criar o arquivo do membro '%s'.\n", nome_membro);
            continue; // Pula para o próximo membro a ser extraído
        }

        // Obtém o tamanho do membro a ser extraído
        long tamanho_membro = membro->tamanho;

        // Define o tamanho do buffer para a leitura em blocos de 1024 bytes
        unsigned char buffer[MAX_CONTEUDO];

        // Calcula o número de leituras completas de 1024 bytes a serem feitas
        int quociente = tamanho_membro / MAX_CONTEUDO;

        // Realiza as leituras completas de 1024 bytes
        for (int j = 0; j < quociente; j++) {
            size_t bytes_lidos = fread(buffer, sizeof(unsigned char), MAX_CONTEUDO, arquivo_archive);

            if (bytes_lidos == 0) {
                printf("Erro ao ler o conteúdo do arquivo.\n");
                fclose(arquivo_membro);
                remove(nome_membro);
                break;
            }
            size_t bytes_gravados = fwrite(buffer, sizeof(unsigned char), bytes_lidos, arquivo_membro);

            if (bytes_gravados < bytes_lidos) {
                printf("Erro ao gravar o conteúdo no arquivo extraído.\n");
                fclose(arquivo_membro);
                remove(nome_membro);
                break;
            }
        }

        // Calcula o tamanho restante (resto) para a leitura final, se houver
        int resto = tamanho_membro % MAX_CONTEUDO;

        // Realiza a leitura final, se houver resto
        if (resto > 0) {
            size_t bytes_lidos = fread(buffer, sizeof(unsigned char), resto, arquivo_archive);

            if (bytes_lidos == 0) {
                printf("Erro ao ler o conteúdo do arquivo.\n");
                fclose(arquivo_membro);
                remove(nome_membro);
                continue; // Pula para o próximo membro a ser extraído
            }

            size_t bytes_gravados = fwrite(buffer, sizeof(unsigned char), bytes_lidos, arquivo_membro);

            if (bytes_gravados < bytes_lidos) {
                printf("Erro ao gravar o conteúdo no arquivo extraído.\n");
                fclose(arquivo_membro);
                remove(nome_membro);
                continue; // Pula para o próximo membro a ser extraído
            }
        }

        // Fecha o arquivo do membro
        fclose(arquivo_membro);
    }

    // Fecha o arquivo de archive
    fclose(arquivo_archive);
}

