#include "archiver.h"

void listar_membros(const ArchiveData *archiveData) {
    printf("Membros no ArchiveData:\n");
    for (int i = 0; i < archiveData->diretorio.num_membros; i++) {
        const Membro *membro = &archiveData->diretorio.membros[i];
        printf("Nome: %s\n", membro->nome);
        printf("Tamanho: %ld bytes\n", membro->tamanho);
        printf("Data de modificacao: %s", ctime(&membro->data_modificacao));
        // Imprimir outras informações desejadas...
    }
}

// Verifica se o arquivo já existe.
int arquivo_existe(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        return 0;  // Arquivo não existe
    }
    fclose(arquivo);
    return 1;  // Arquivo existe
}

void verificar_conteudo_membro(const char *nome_arquivo, const ConteudoMembro *conteudo_membro, size_t tamanho_arquivo) {
    // Abre o arquivo original em modo de leitura binária
    FILE *arquivo_original = fopen(nome_arquivo, "rb");
    if (arquivo_original == NULL) {
        printf("Erro ao abrir o arquivo original '%s' para verificação.\n", nome_arquivo);
        return;
    }

    // Lê o conteúdo do arquivo original
    unsigned char *conteudo_original = malloc(sizeof(unsigned char) * tamanho_arquivo);
    if (conteudo_original == NULL) {
        printf("Erro ao alocar memória para o conteúdo original do arquivo '%s'.\n", nome_arquivo);
        fclose(arquivo_original);
        return;
    }

    size_t bytes_lidos_original = fread(conteudo_original, sizeof(unsigned char), tamanho_arquivo, arquivo_original);
    if (bytes_lidos_original != tamanho_arquivo) {
        printf("Erro ao ler o conteúdo do arquivo original '%s' para verificação.\n", nome_arquivo);
        fclose(arquivo_original);
        free(conteudo_original);
        return;
    }

    // Fecha o arquivo original
    fclose(arquivo_original);

    // Compara o conteúdo do arquivo original com o conteúdo armazenado na estrutura ConteudoMembro
    if (memcmp(conteudo_original, conteudo_membro->conteudo, tamanho_arquivo) == 0) {
        printf("O conteúdo do arquivo '%s' foi armazenado corretamente na estrutura ConteudoMembro.\n", nome_arquivo);
    } else {
        printf("Erro: o conteúdo do arquivo '%s' não foi copiado corretamente para a estrutura ConteudoMembro.\n", nome_arquivo);
    }

    // Libera a memória alocada para o conteúdo original do arquivo
    free(conteudo_original);
}

ArchiveData* inicializar_archive(const char *nome_archive) {
    ArchiveData* archiveData = malloc(sizeof(ArchiveData));
    if (archiveData == NULL) {
        printf("Erro ao alocar memória para o ArchiveData.\n");
        return NULL;
    }

    // Verifica se o arquivo já existe
    if (arquivo_existe(nome_archive)) {
        printf("O arquivo '%s' já existe.\n", nome_archive);

        // Abrir o arquivo em modo de leitura binária
        FILE *arquivo = fopen(nome_archive, "rb");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo.\n");
            free(archiveData);
            return NULL;
        }

        // Ler os dados do arquivo para preencher a estrutura ArchiveData
        fread(archiveData, sizeof(ArchiveData), 1, arquivo);

        fclose(arquivo);
    } else {
        printf("O arquivo '%s' não existe.\n", nome_archive);

        strcpy(archiveData->nome_archive, nome_archive);
        archiveData->diretorio.num_membros = 0;

        FILE *arquivo = fopen(nome_archive, "wb");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo.\n");
            free(archiveData);
            return NULL;
        }

        // Define a posição inicial do arquivo
        fseek(arquivo, 0, SEEK_SET);

        // Escreve a estrutura ArchiveData no arquivo
        fwrite(archiveData, sizeof(ArchiveData), 1, arquivo);

        fclose(arquivo);

        printf("ArchiveData '%s' criado com sucesso!\n", nome_archive);
    }

    return archiveData;
}

int verifica_nome(const ArchiveData *archiveData, const char *nome_arquivo) {
    for (int i = 0; i < archiveData->diretorio.num_membros; i++) {
        if (strcmp(nome_arquivo, archiveData->diretorio.membros[i].nome) == 0) {
            return 1;  // O nome do arquivo já existe no Archive
        }
    }
    return 0;  // O nome do arquivo não existe no Archive
}

void extrair_informacoes_membro(const char *nome_arquivo, Membro *membro) {
    struct stat info_arquivo;

    // Verifica se o arquivo existe e obtém as informações
    if (stat(nome_arquivo, &info_arquivo) == -1) {
        perror("Erro ao obter informações do arquivo");
        return;
    }

    // Preenche as informações no membro
    strncpy(membro->nome, nome_arquivo, MAX_NOME - 1);
    membro->nome[MAX_NOME - 1] = '\0';

    // Remove os espaços do nome do arquivo
    const char *ptr = nome_arquivo;
    char *dest = membro->nome;
    while (*ptr) {
        if (*ptr != ' ') {
            *dest++ = *ptr;
        }
        ptr++;
    }
    *dest = '\0';

    membro->user_ID = info_arquivo.st_uid;
    membro->permissoes = info_arquivo.st_mode & 0777;
    membro->tamanho = info_arquivo.st_size;
    membro->data_modificacao = info_arquivo.st_mtime;
    membro->ordem = 0; // Defina o valor adequado para a ordem do membro
    membro->localizacao = 0; // Defina o valor adequado para a localização do membro
}

int contar_membros(char **membros) {
    int contador = 0;
    while (membros[contador] != NULL) {
        contador++;
    }
    return contador;
}

int verificar_tempo_modificacao_mais_recente(const char* nome_membro, Membro* membro_existente) {
    struct stat arquivo_stat;
    if (stat(nome_membro, &arquivo_stat) != 0) {
        // Erro ao obter informações do arquivo
        return 0;
    }

    // Verificar se o tempo de modificação do novo membro é mais recente
    if (arquivo_stat.st_mtime > membro_existente->data_modificacao) {
        return 1;
    }

    return 0;
}

void substituir_recente(Archiver* archiver, int posicao_existente, Membro* novo_membro, unsigned char* conteudo, long tamanho_conteudo) {
    // Substituir o membro existente na posição especificada pelo novo membro
    Membro* membro_existente = &(archiver->archiveData.diretorio.membros[posicao_existente]);
    strcpy(membro_existente->nome, novo_membro->nome);
    membro_existente->user_ID = novo_membro->user_ID;
    membro_existente->permissoes = novo_membro->permissoes;
    membro_existente->tamanho = novo_membro->tamanho;
    membro_existente->data_modificacao = novo_membro->data_modificacao;
    membro_existente->ordem = novo_membro->ordem;
    membro_existente->localizacao = novo_membro->localizacao;

    // Atualizar o conteúdo do arquivo
    memcpy(archiver->archiveData.conteudo.conteudo, conteudo, tamanho_conteudo);
}

