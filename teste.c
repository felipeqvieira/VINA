#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_NOME 256
#define QUANTIDADE_MAXIMA 100
#define MAX_CONTEUDO 1024

typedef struct {
    char nome[MAX_NOME];
    int user_ID;
    int permissoes;
    size_t tamanho;
    time_t data_modificacao;
    int ordem;
    int localizacao;
} Membro;

typedef struct {
    int num_membros;
    Membro membros[QUANTIDADE_MAXIMA];
} Diretorio;

typedef struct {
    char nome_archive[MAX_NOME];
    Diretorio diretorio;
} ArchiveData;

typedef struct {
    unsigned char conteudo[MAX_CONTEUDO];
} ConteudoMembro;

typedef struct {
    ArchiveData archiveData;
    ConteudoMembro conteudo;
} Archiver;

int arquivo_existe(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        return 0;  // Arquivo não existe
    }
    fclose(arquivo);
    return 1;  // Arquivo existe
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

int verifica_nome(const ArchiveData *archiveData, const char *nome_arquivo) {
    for (int i = 0; i < archiveData->diretorio.num_membros; i++) {
        if (strcmp(nome_arquivo, archiveData->diretorio.membros[i].nome) == 0) {
            return 1;  // O nome do arquivo já existe no Archive
        }
    }
    return 0;  // O nome do arquivo não existe no Archive
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

int main() {
    const char *nome_arquivo = "arquivo.txt";
    const char *nome_archive = "archive.bin";

    // Verifica se o arquivo original existe
    if (!arquivo_existe(nome_arquivo)) {
        printf("O arquivo original '%s' não existe.\n", nome_arquivo);
        return 1;
    }

    // Verifica se o arquivo do Archive existe
    if (!arquivo_existe(nome_archive)) {
        printf("O arquivo do Archive '%s' não existe.\n", nome_archive);
        return 1;
    }

    // Lê o arquivo do Archive
    FILE *arquivo_archive = fopen(nome_archive, "rb");
    if (arquivo_archive == NULL) {
        perror("Erro ao abrir o arquivo do Archive");
        return 1;
    }

    // Lê os dados do Archive
    Archiver archiver;
    fread(&archiver, sizeof(Archiver), 1, arquivo_archive);
    fclose(arquivo_archive);

    // Extrai as informações do membro
    Membro membro;
    extrair_informacoes_membro(nome_arquivo, &membro);

    // Verifica se o nome do arquivo já existe no Archive
    if (verifica_nome(&archiver.archiveData, membro.nome)) {
        printf("O nome do arquivo '%s' já existe no Archive.\n", membro.nome);
        return 1;
    }

    // Verifica o conteúdo do membro
    verificar_conteudo_membro(nome_arquivo, &archiver.conteudo, membro.tamanho);

    printf("Verificação concluída com sucesso.\n");

    return 0;
}
