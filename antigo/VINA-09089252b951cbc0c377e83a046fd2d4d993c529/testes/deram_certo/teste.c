#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME_ARCHIVE 100
#define QUANTIDADE_MAXIMA 1000
#define MAX_NOME 100
#define MAX_CONTEUDO 1000000

typedef struct {
    char nome[MAX_NOME];
    int user_ID;
    int permissoes;
    long tamanho;
    time_t data_modificacao;
    int ordem;
    long localizacao;
} Membro;

typedef struct {
    Membro membros[QUANTIDADE_MAXIMA];
    int num_membros;
} Diretorio;

typedef struct {
    unsigned char conteudo[MAX_CONTEUDO];
} ConteudoMembro;

typedef struct {
    Diretorio diretorio;
    ConteudoMembro conteudo;
    char nome_archive[MAX_NOME_ARCHIVE];
} ArchiveData;

typedef struct {
    ArchiveData archiveData;
} Archiver;

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

    if (arquivo_existe(nome_archive) != 0) {
        printf("O arquivo '%s' já existe.\n", nome_archive);
        return NULL;
    }

    printf("O arquivo '%s' nao existe.\n", nome_archive);

    ArchiveData* archiveData = malloc(sizeof(ArchiveData));
    if (archiveData == NULL) {
        printf("Erro ao alocar memória para o ArchiveData.\n");
        return NULL;
    }

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

void inserir_membros(Archiver *archiver, const char *nome_membro) {

    // Verifica se o arquivo já existe no ArchiveData
    if (verifica_nome(&(archiver->archiveData), nome_membro)) {
        printf("O membro '%s' já existe no ArchiveData.\n", nome_membro);
        return;
    }

    printf("oi\n");


    // Verifica se há espaço disponível no diretório
    if (archiver->archiveData.diretorio.num_membros >= QUANTIDADE_MAXIMA) {
        printf("Não há espaço disponível para adicionar o membro.\n");
        return;
    }

    // Cria uma nova estrutura de Membro para armazenar as informações
    Membro novo_membro;
    extrair_informacoes_membro(nome_membro, &novo_membro);

    // Insere o novo membro na próxima posição disponível
    int posicao_insercao = archiver->archiveData.diretorio.num_membros;
    Membro *membro = &(archiver->archiveData.diretorio.membros[posicao_insercao]);
    memcpy(membro, &novo_membro, sizeof(Membro));

    // Atualiza o número de membros no diretório
    archiver->archiveData.diretorio.num_membros++;

    // Abre o arquivo do membro em modo de leitura binária
    FILE *arquivo_membro = fopen(nome_membro, "rb");
    if (arquivo_membro == NULL) {
        printf("Erro ao abrir o arquivo do membro '%s'.\n", nome_membro);
        return;
    }

    // Lê o conteúdo do membro e o armazena na estrutura ConteudoMembro
    size_t bytes_lidos = fread(archiver->archiveData.conteudo.conteudo, sizeof(unsigned char), MAX_CONTEUDO, arquivo_membro);
    if (bytes_lidos == 0) {
        printf("Erro ao ler o conteúdo do membro '%s'.\n", nome_membro);
        fclose(arquivo_membro);
        return;
    }

    // Fecha o arquivo do membro
    fclose(arquivo_membro);

    printf("Membro '%s' inserido com sucesso no ArchiveData.\n", nome_membro);

     // Verifica se o conteúdo do arquivo foi copiado corretamente
    verificar_conteudo_membro(nome_membro, &(archiver->archiveData.conteudo), bytes_lidos);
}

int main() {

    const char *nome_archive = "felipe.vpp";

    Archiver *archiver = malloc(sizeof(Archiver));
    if (archiver == NULL) {
        printf("Erro ao alocar memoria para o Archiver.\n");
        return 1;
    }

    ArchiveData *archiveData = inicializar_archive(nome_archive);
    if (archiveData == NULL) {
        printf("Erro ao inicializar o ArchiveData.\n");
        free(archiver);
        return 1;
    }

    archiver->archiveData = *archiveData;
       
    inserir_membros(archiver, "arquivo1.txt");

    listar_membros(&(archiver->archiveData));

    free(archiver);
    
    
    return 0;
}
