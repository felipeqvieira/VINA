#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NOME_ARQUIVO 100
#define MAX_CAMINHO_ARQUIVO 100

#define ARQUIVO_VERIFICADO 0 // constante caso sucesso
#define ERRO_NA_ABERTURA 1
#define ARQUIVO_NAO_ENCONTRADO 2
#define ERRO_FSEEK 3
#define ERRO_FTELL 4
#define QUANTIDADE_MAXIMA 1000 // quantidade maxima de arquivos no diretorio
#define BUFFER_SIZE 1024
#define MAX_NOME_ARCHIVE 1024

typedef struct { 

    char nome[100];
    int user_ID;
    int permissoes;
    long tamanho;
    time_t data_modificacao;
    int ordem;
    long localizacao;

}Membro;

typedef struct {
    Membro* membros[QUANTIDADE_MAXIMA];
    int num_membros;
    char nome_archive[MAX_NOME_ARCHIVE];
} Archive;

typedef struct {

    Archive archive;

}Archiver;

void preencher_dados_membro(Membro* membro, const char* nome_arquivo){

    struct stat file_info;

    if (stat(nome_arquivo, &file_info) == -1) { // Alteração aqui
        perror("Erro ao obter informações do arquivo");
        return;
    }

    membro->tamanho = file_info.st_size;
    membro->data_modificacao = file_info.st_mtime;
    membro->user_ID = file_info.st_uid;
    membro->permissoes = (file_info.st_mode & S_IRWXU) | 0070 | 0007;
}

void listar_conteudo(const char* nome_arquivo) {
    FILE* arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para leitura");
        return;
    }

    Archiver archiver;

    // Ler o arquivo completo para a estrutura Archiver
    fread(&archiver, sizeof(Archiver), 1, arquivo);

    Archive* archive = &(archiver.archive);

    printf("Conteúdo do arquivo '%s':\n", nome_arquivo);

    // Voltar para o início do arquivo (após a estrutura Archiver)
    fseek(arquivo, sizeof(Archiver), SEEK_SET);

    // Ler cada membro individualmente do arquivo
    for (int i = 0; i < archive->num_membros; i++) {
        Membro membro;

        fread(&membro, sizeof(Membro), 1, arquivo);

        // Imprimir os dados do membro
        printf("Nome: %s\n", membro.nome);
        printf("Tamanho: %ld bytes\n", membro.tamanho);
        printf("Propriedades: %d\n", membro.permissoes);
        printf("---------------\n");
    }

    fclose(arquivo);
}

void adicionar_membro(Archive* archive, const char* nome_membro, const char* nome_arquivo_destino) {
    char nome_arquivo[MAX_NOME_ARQUIVO];
    char caminho_arquivo[MAX_CAMINHO_ARQUIVO];

    // Verificar se o nome do arquivo contém um caminho
    const char* separador = strrchr(nome_membro, '/');
    if (separador != NULL) {
        // O nome do arquivo contém um caminho
        strcpy(caminho_arquivo, nome_membro);
        strncpy(nome_arquivo, separador + 1, MAX_NOME_ARQUIVO - 1);
        caminho_arquivo[separador - nome_membro + 1] = '\0';
    } else {
        // O nome do arquivo não contém um caminho
        strcpy(nome_arquivo, nome_membro);
        caminho_arquivo[0] = '\0'; // Caminho vazio
    }

    // Construir o caminho completo do arquivo destino
    char caminho_completo[MAX_CAMINHO_ARQUIVO + MAX_NOME_ARQUIVO];
    if (caminho_arquivo[0] != '\0') {
        snprintf(caminho_completo, sizeof(caminho_completo), "%s%s", nome_arquivo_destino, caminho_arquivo);
    } else {
        strncpy(caminho_completo, nome_arquivo_destino, sizeof(caminho_completo) - 1);
    }

    // Abrir o arquivo de destino em modo de escrita binária
    FILE* arquivo_destino = fopen(caminho_completo, "ab");
    if (arquivo_destino == NULL) {
        perror("Erro ao abrir o arquivo de destino");
        return;
    }

    // Restante do código para adicionar o membro ao arquivo de destino
    char buffer[BUFFER_SIZE];
    size_t bytes_lidos;

    FILE* membro = fopen(nome_membro, "rb");
    if (membro == NULL) {
        perror("Erro ao abrir o arquivo de membro de origem");
        fclose(arquivo_destino);
        return;
    }

    fseek(arquivo_destino, 0, SEEK_END); // Posicionar o cursor no final do arquivo de destino

    long localizacao = ftell(arquivo_destino); // Obter a localização atual no arquivo

    while ((bytes_lidos = fread(buffer, 1, BUFFER_SIZE, membro)) > 0) {
        // Gravar os dados do buffer no arquivo de destino
        fwrite(buffer, 1, bytes_lidos, arquivo_destino);
    }

    fclose(membro);
    fclose(arquivo_destino);

    // Preencher os dados do membro com base nas informações reais do arquivo
    Membro novo_membro;
    strncpy(novo_membro.nome, nome_arquivo, MAX_NOME_ARQUIVO);

    // Preencher os dados do membro com base nas informações reais do arquivo
    preencher_dados_membro(&novo_membro, nome_membro);

    novo_membro.ordem = archive->num_membros;
    novo_membro.localizacao = localizacao;
    novo_membro.tamanho = ftell(membro); // Definir o tamanho real do membro

    // Adicionar o membro ao arquivo de destino
    FILE* arquivo_destino_membro = fopen(nome_arquivo_destino, "ab");
    if (arquivo_destino_membro == NULL) {
        perror("Erro ao abrir o arquivo de destino");
        return;
    }

    fwrite(&novo_membro, sizeof(Membro), 1, arquivo_destino_membro);

    fclose(arquivo_destino_membro);

    // Alocar memória para o novo membro e copiar os dados
    Membro* membro_alocado = (Membro*)malloc(sizeof(Membro));
    if (membro_alocado == NULL) {
        perror("Erro ao alocar memória para o novo membro");
        return;
    }

    memcpy(membro_alocado, &novo_membro, sizeof(Membro));

    archive->membros[archive->num_membros] = membro_alocado;
    archive->num_membros++;
}


void testar_adicionar_membro(const char* nome_archive, const char* nome_membro) {
    Archive archive;

    FILE* arquivo_archive = fopen(nome_archive, "rb+");
    if (arquivo_archive == NULL) {
        perror("Erro ao abrir o arquivo de archive");
        return;
    }

    fread(&archive, sizeof(Archive), 1, arquivo_archive);

    adicionar_membro(&archive, nome_membro, nome_archive);

    rewind(arquivo_archive);
    fwrite(&archive, sizeof(Archive), 1, arquivo_archive);

    fclose(arquivo_archive);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Uso: %s <nome_arquivo_archive> <nome_arquivo_membro>\n", argv[0]);
        return 1;
    }

    const char* nome_archive = argv[1];
    const char* nome_membro = argv[2];

    testar_adicionar_membro(nome_archive, nome_membro);

    listar_conteudo(nome_archive);

    return 0;
}
