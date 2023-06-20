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
#define MAX_CONTEUDO 1024

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
    Diretorio diretorio;
} ArchiveData;

typedef struct {
    ArchiveData archiveData;
} Archiver;

void listar_membros(const Archiver *archiver) {
    printf("Membros no ArchiveData:\n");
    printf("-----------------------\n");

    for (int i = 0; i < archiver->archiveData.diretorio.num_membros; i++) {
        const Membro *membro = &(archiver->archiveData.diretorio.membros[i]);
        printf("Nome: %s\n", membro->nome);
        printf("Usuário ID: %d\n", membro->user_ID);
        printf("Permissões: %d\n", membro->permissoes);
        printf("Tamanho: %ld\n", membro->tamanho);
        printf("Data de modificação: %s", ctime(&(membro->data_modificacao)));
        printf("Ordem: %d\n", membro->ordem);
        printf("Localização: %ld\n", membro->localizacao);
        printf("-----------------------\n");
    }
}


int contar_membros(char **membros) {
    int contador = 0;
    while (membros[contador] != NULL) {
        contador++;
    }
    return contador;
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

void verificar_conteudo_membro(const char *nome_membro, ArchiveData *archiveData, size_t tamanho) {
    
    for (int i = 0; i < archiveData->diretorio.num_membros; i++) {
        Membro *membro = &(archiveData->diretorio.membros[i]);
        if (strcmp(membro->nome, nome_membro) == 0) {
            if (membro->tamanho == tamanho) {
                printf("O conteúdo do membro '%s' foi copiado corretamente.\n", nome_membro);
            } else {
                printf("O tamanho do conteúdo do membro '%s' não corresponde ao tamanho original.\n", nome_membro);
            }
            return;
        }
    }

    printf("O membro '%s' não foi encontrado no ArchiveData.\n", nome_membro);
}

int verifica_nome(const ArchiveData *archiveData, const char *nome_arquivo) {

    printf("numero de membros: %d\n", archiveData->diretorio.num_membros);

    for (int i = 0; i < archiveData->diretorio.num_membros; i++) {
        printf("membros: %s\n", archiveData->diretorio.membros[i].nome);
        if (archiveData->diretorio.membros[i].nome[0] != '\0' && strcmp(nome_arquivo, archiveData->diretorio.membros[i].nome) == 0) {
            return 1;  // O nome do arquivo já existe no Archive
        }
    }
    return 0;  // O nome do arquivo não existe no Archive
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
        fread(&(archiveData->diretorio), sizeof(Diretorio), 1, arquivo);

        // Ler informações dos arquivos existentes e atualizar os membros correspondentes
        for (int i = 0; i < archiveData->diretorio.num_membros; i++) {
            Membro *membro = &(archiveData->diretorio.membros[i]);

            // Verifica se o arquivo existe
            if (arquivo_existe(membro->nome)) {
                // Extrair informações do arquivo existente e atualizar o membro
                extrair_informacoes_membro(membro->nome, membro);
            } else {
                printf("O arquivo '%s' não existe.\n", membro->nome);
                // Marcar o nome do membro como inválido
                membro->nome[0] = '\0';
            }
        }

        fclose(arquivo);
    } else {
        printf("O arquivo '%s' não existe.\n", nome_archive);

        archiveData->diretorio.num_membros = 0;

        FILE *arquivo = fopen(nome_archive, "wb");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo.\n");
            free(archiveData);
            return NULL;
        }

        // Define a posição inicial do arquivo
        fseek(arquivo, 0, SEEK_SET);

        // Escreve a estrutura Diretorio no arquivo
        fwrite(&(archiveData->diretorio), sizeof(Diretorio), 1, arquivo);

        fclose(arquivo);

        printf("ArchiveData '%s' criado com sucesso!\n", nome_archive);
    }

    return archiveData;
}

void inserir_membros(Archiver *archiver, char **nomes_arquivos, int num_arquivos) {
    for (int i = 0; i < num_arquivos; i++) {
        char *nome_membro = nomes_arquivos[i];

        // Verifica se o arquivo já existe no ArchiveData
        if (verifica_nome(&(archiver->archiveData), nome_membro)) {
            printf("O membro '%s' já existe no ArchiveData.\n", nome_membro);
            continue;  // Passa para o próximo arquivo
        }

        printf("O membro '%s' não existe no ArchiveData.\n", nome_membro);

        // Verifica se há espaço disponível no diretório
        if (archiver->archiveData.diretorio.num_membros >= QUANTIDADE_MAXIMA) {
            printf("Não há espaço disponível para adicionar o membro '%s'.\n", nome_membro);
            continue;  // Passa para o próximo arquivo
        }

        printf("Há espaço disponível para adicionar o membro '%s'.\n", nome_membro);

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
            continue;  // Passa para o próximo arquivo
        }

        printf("Arquivo do membro '%s' aberto.\n", nome_membro);

        // Cria o nome do arquivo dentro do archiver
        char nome_arquivo_archiver[MAX_NOME];
        snprintf(nome_arquivo_archiver, MAX_NOME, "%s", nome_membro);

        printf("nome arquivo archiver: %s\n", nome_arquivo_archiver);

        // Abre o arquivo do membro dentro do archiver em modo de escrita binária
        FILE *arquivo_archiver = fopen(nome_arquivo_archiver, "wb");
        if (arquivo_archiver == NULL) {
            printf("Erro ao criar o arquivo do membro '%s' dentro do ArchiveData.\n", nome_membro);
            fclose(arquivo_membro);
            continue;  // Passa para o próximo arquivo
        }

        // Lê e insere o conteúdo do membro em blocos de 1024 bytes
        unsigned char buffer[MAX_CONTEUDO];
        size_t bytes_lidos;
        while ((bytes_lidos = fread(buffer, sizeof(unsigned char), MAX_CONTEUDO, arquivo_membro)) > 0) {
            fwrite(buffer, sizeof(unsigned char), bytes_lidos, arquivo_archiver);
        }

        // Fecha o arquivo do membro dentro do archiver
        fclose(arquivo_archiver);

        // Fecha o arquivo do membro original
        fclose(arquivo_membro);

        // Verifica se o conteúdo do arquivo foi copiado corretamente
        verificar_conteudo_membro(nome_arquivo_archiver, &(archiver->archiveData), novo_membro.tamanho);

        printf("Membro '%s' inserido com sucesso no ArchiveData.\n", nome_membro);
    }
}

int main(int argc, char *argv[]) {
    // Verificar se foram fornecidos argumentos suficientes
    if (argc < 3) {
        printf("Uso: vina++ <opção> <archive> [membro1 membro2 ...]\n");
        return 1;
    }

    char *opcao = argv[1];
    char *archive = argv[2];
    char **membros = argv + 3;  // Pode ser vazio, dependendo da opção

    // Convertendo caminhos absolutos para caminhos relativos
    char *diretorio_atual = getcwd(NULL, 0);  // Obtém o diretório atual
    int tamanho_diretorio_atual = strlen(diretorio_atual);

    for (int i = 0; membros[i] != NULL; i++) {
        if (membros[i][0] != '/') {
            // O membro não é um caminho absoluto, ignora a conversão
            continue;
        }

        char *caminho_relativo = malloc(strlen(membros[i]) + 2 + tamanho_diretorio_atual);
        if (caminho_relativo == NULL) {
            printf("Erro ao alocar memória.\n");
            return 1;
        }

        strcpy(caminho_relativo, ".");
        strcat(caminho_relativo, membros[i]);

        membros[i] = caminho_relativo;
    }

    free(diretorio_atual);

    Archiver *archiver = malloc(sizeof(Archiver));
    if (archiver == NULL) {
        printf("Erro ao alocar memoria para o Archiver.\n");
        return 1;
    }

    ArchiveData *archiveData = inicializar_archive(archive);
    if (archiveData == NULL) {
        printf("Erro ao inicializar o ArchiveData.\n");
        free(archiver);
        return 1;
    }

    archiver->archiveData = *archiveData;

    int num_membros = contar_membros(membros);

    // Encaminhar para a função apropriada de acordo com a opção
    if (strcmp(opcao, "-i") == 0) {
        inserir_membros(archiver, membros, num_membros);
    }/* else if (strcmp(opcao, "-a") == 0) {
        inserir_data(archive, membros, num_membros);
    
    } else if (strcmp(opcao, "-m") == 0) {
        mover_membro(archive, membros);
    } else if (strcmp(opcao, "-x") == 0) {
        extrair_membros(archive, membros);
    } else if (strcmp(opcao, "-r") == 0) {
        remover_membros(archive, membros);
    } else if (strcmp(opcao, "-c") == 0) {
        listar_conteudo(archive);
    } else if (strcmp(opcao, "-h") == 0) {
        exibir_ajuda();
    } else {
        printf("Opção inválida.\n");
        exibir_ajuda();
    }
    */

    listar_membros(archiver);

    return 0;
}