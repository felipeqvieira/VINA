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




/* MEMBROS*/

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




/* ARCHIVE */

void verificar_archive_existente(Archiver *archiver, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        printf("O arquivo '%s' não existe. Um novo arquivo será criado.\n", nome_arquivo);
        archiver->archiveData.diretorio.num_membros = 0;

        // Inicializar o diretório alocando memória
        memset(archiver->archiveData.diretorio.membros, 0, sizeof(archiver->archiveData.diretorio.membros));

        return;
    }

    fseek(arquivo, -sizeof(Diretorio), SEEK_END);
    fread(&(archiver->archiveData.diretorio), sizeof(Diretorio), 1, arquivo);
    fclose(arquivo);

    printf("Archive '%s' encontrado. Diretório carregado com sucesso.\n", nome_arquivo);
}

void verificar_conteudo_membro(const Archiver *archiver, int indice_membro) {
    // Verifica se o índice do membro é válido
    if (indice_membro < 0 || indice_membro >= archiver->archiveData.diretorio.num_membros) {
        printf("Índice do membro inválido.\n");
        return;
    }

    // Obtém o nome do arquivo do membro
    char nome_membro[MAX_NOME];
    strncpy(nome_membro, archiver->archiveData.diretorio.membros[indice_membro].nome, MAX_NOME);

    // Abre o arquivo do membro original em modo de leitura binária
    FILE *arquivo_membro = fopen(nome_membro, "rb");
    if (arquivo_membro == NULL) {
        printf("Erro ao abrir o arquivo original '%s' para verificação do conteúdo do membro.\n", nome_membro);
        return;
    }

    // Calcula o tamanho do membro original
    fseek(arquivo_membro, 0, SEEK_END);
    long tamanho_membro_original = ftell(arquivo_membro);
    rewind(arquivo_membro);

    // Abre o arquivo do archive em modo de leitura binária
    FILE *arquivo_archive = fopen(archiver->archiveData.diretorio.membros[indice_membro].nome, "rb");
    if (arquivo_archive == NULL) {
        printf("Erro ao abrir o arquivo do archive '%s' para verificação do conteúdo do membro.\n", archiver->archiveData.diretorio.membros[indice_membro].nome);
        fclose(arquivo_membro);
        return;
    }

    // Calcula a posição onde o membro está localizado
    long posicao_membro = archiver->archiveData.diretorio.membros[indice_membro].localizacao;

    // Move o cursor para a posição do membro
    fseek(arquivo_archive, posicao_membro, SEEK_SET);

    // Lê o conteúdo do membro do arquivo do archive
    unsigned char buffer[MAX_CONTEUDO];
    size_t bytes_lidos = fread(buffer, sizeof(unsigned char), archiver->archiveData.diretorio.membros[indice_membro].tamanho, arquivo_archive);

    // Fecha o arquivo do archive
    fclose(arquivo_archive);

    // Lê o conteúdo do membro original
    unsigned char buffer_original[MAX_CONTEUDO];
    size_t bytes_lidos_original = fread(buffer_original, sizeof(unsigned char), bytes_lidos, arquivo_membro);

    // Fecha o arquivo do membro original
    fclose(arquivo_membro);

    // Compara o conteúdo do membro do arquivo do archive com o conteúdo original
    if (bytes_lidos == bytes_lidos_original && memcmp(buffer, buffer_original, bytes_lidos) == 0 && bytes_lidos == tamanho_membro_original) {
        printf("O conteúdo do membro '%s' no arquivo do archive está correto.\n", nome_membro);
    } else {
        printf("O conteúdo do membro '%s' no arquivo do archive está incorreto.\n", nome_membro);
    }
}

void inserir_membros(Archiver *archiver, char *nome_archive, char **nomes_arquivos, int num_arquivos) {
    // Abre o arquivo do archive em modo de leitura e escrita binária
    FILE *arquivo_archive = fopen(nome_archive, "rb+");
    if (arquivo_archive == NULL) {
        printf("Erro ao abrir o arquivo do archive '%s'.\n", nome_archive);
        return;
    }

    // Move o cursor para o início do arquivo
    fseek(arquivo_archive, 0, SEEK_SET);

    // Insere os novos membros no início do arquivo
    for (int i = 0; i < num_arquivos; i++) {
        char *nome_membro = nomes_arquivos[i];

        // Verifica se o membro já existe no ArchiveData
        int membro_existente = 0;
        for (int j = 0; j < archiver->archiveData.diretorio.num_membros; j++) {
            Membro *membro = &(archiver->archiveData.diretorio.membros[j]);
            if (strcmp(membro->nome, nome_membro) == 0) {
                printf("O membro '%s' já existe no ArchiveData.\n", nome_membro);
                membro_existente = 1;
                break;
            }
        }
        if (membro_existente) {
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

        // Abre o arquivo do membro em modo de leitura binária
        FILE *arquivo_membro = fopen(nome_membro, "rb");
        if (arquivo_membro == NULL) {
            printf("Erro ao abrir o arquivo do membro '%s'.\n", nome_membro);
            continue;  // Passa para o próximo arquivo
        }

        printf("Arquivo membro '%s' aberto.\n", nome_membro);

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
        archiver->archiveData.diretorio.membros[archiver->archiveData.diretorio.num_membros] = novo_membro;
        archiver->archiveData.diretorio.num_membros++;
    }

    // Move o cursor para o início do diretório
    fseek(arquivo_archive, sizeof(ArchiveData), SEEK_SET);

    // Escreve o novo diretório no arquivo
    fwrite(&(archiver->archiveData.diretorio), sizeof(Diretorio), 1, arquivo_archive);

    // Fecha o arquivo do archive
    fclose(arquivo_archive);
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

    verificar_archive_existente(archiver, archive);

    int num_membros = contar_membros(membros);

    // Encaminhar para a função apropriada de acordo com a opção
    if (strcmp(opcao, "-i") == 0) {
        inserir_membros(archiver, archive, membros, num_membros);
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
    verificar_conteudo_membro(archiver, 0);

    return 0;
}