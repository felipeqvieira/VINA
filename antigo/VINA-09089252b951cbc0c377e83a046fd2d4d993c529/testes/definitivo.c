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

int verifica_nome(Archiver *archive, const char *nome_arquivo) {
    for (int i = 0; i < archive->archiveData.diretorio.num_membros; i++) {
        if (strcmp(nome_arquivo, archive->archiveData.diretorio.membros[i].nome) == 0) {
            return i;  // O nome do arquivo já existe no Archive
        }
    }
    return -1;  // O nome do arquivo não existe no Archive
}

void listar_membros(const Archiver *archiver) {
    printf("\n-----------------------\n");
    printf("Membros no ArchiveData:");
    printf("\n-----------------------\n");

    for (int i = 0; i < archiver->archiveData.diretorio.num_membros; i++) {
        const Membro *membro = &(archiver->archiveData.diretorio.membros[i]);
        printf("Nome: %s\n", membro->nome);
        printf("Usuario ID: %d\n", membro->user_ID);
        printf("Permissoes: %d\n", membro->permissoes);
        printf("Tamanho: %ld\n", membro->tamanho);
        printf("Data de modificacao: %s", ctime(&(membro->data_modificacao)));
        printf("Ordem: %d\n", membro->ordem);
        printf("Localizacao: %ld\n", membro->localizacao);
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

void extrair_informacoes_membro(Archiver *archiver, const char *nome_arquivo, Membro *membro) {
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
    membro->ordem = archiver->archiveData.diretorio.num_membros; 
    membro->localizacao = archiver->archiveData.diretorio.num_membros; 
}




/* ARCHIVE */

void verificar_archive_existente(Archiver *archiver, const char *nome_arquivo) {

    printf("\n----------------------------------\n");
    printf("VERFICANDO ARCHIVE EXISTENTE");
    printf("\n----------------------------------\n");
    FILE *arquivo = fopen(nome_arquivo, "r+b");
    if (arquivo == NULL) {
        printf("O arquivo '%s' nao existe. Um novo arquivo sera criado.\n", nome_arquivo);

        // Cria um novo arquivo vazio
        arquivo = fopen(nome_arquivo, "w+b");
        if (arquivo == NULL) {
            printf("Erro ao criar o arquivo '%s'.\n", nome_arquivo);
            return;
        }

        archiver->archiveData.diretorio.num_membros = 0;

        // Inicializar o diretório alocando memória
        memset(archiver->archiveData.diretorio.membros, 0, sizeof(archiver->archiveData.diretorio.membros));

        // Salva a estrutura do diretório no arquivo
        fwrite(&(archiver->archiveData.diretorio), sizeof(Diretorio), 1, arquivo);

        fclose(arquivo);

        return;
    }

    fseek(arquivo, -sizeof(Diretorio), SEEK_END);
    fread(&(archiver->archiveData.diretorio), sizeof(Diretorio), 1, arquivo);
    fclose(arquivo);

    printf("Archive '%s' encontrado. Diretorio carregado com sucesso.\n", nome_arquivo);
}


void inserir_membros(Archiver *archiver, char *nome_archive, char **nomes_arquivos, int num_arquivos) {

    printf("\n----------------------------------\n");
    printf("INSERINDO MEMBRO");
    printf("\n----------------------------------\n");
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
        extrair_informacoes_membro(archiver, nome_membro, &novo_membro);

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


/*int substituir_data(Archiver *archiver, const char *nome_arquivo) {

    int indice_membro = verifica_nome(archiver, nome_arquivo);

    if (indice_membro != -1) {
        // O arquivo existe no diretório do archive

        // Extrair informações do membro existente
        Membro membro_existente = archiver->archiveData.diretorio.membros[indice_membro];
        struct stat arquivo_stat;
        if (stat(nome_arquivo, &arquivo_stat) != 0) {
            printf("Erro ao obter informações do arquivo '%s'. O arquivo não será substituído.\n", nome_arquivo);
            return -1;
        }
        time_t data_modificacao_arquivo = arquivo_stat.st_mtime;
        time_t data_modificacao_membro = membro_existente.data_modificacao;

        if (data_modificacao_arquivo <= data_modificacao_membro) {
            printf("O arquivo '%s' não será substituído, pois não é mais recente do que o arquivo existente no archive.\n", nome_arquivo);
            return 1;
        }

        // Substituir o membro existente pelo novo arquivo
        substituir_membro(archiver, nome_arquivo);
        return 1;
    }

    // Adicionar o novo arquivo ao archive
    return 0;

}
*/

void atualizar_membros(Archiver *archiver, int indice_membro_extrair) {
    int num_membros = archiver->archiveData.diretorio.num_membros;
    Membro *membros = archiver->archiveData.diretorio.membros;
    FILE *arquivo_archive = archiver->arquivo_archive;

    // Calcular o tamanho do espaço vazio a ser preenchido
    long tamanho_vazio = 0;
    if (indice_membro_extrair < num_membros - 1) {
        long posicao_atual = encontrar_posicao_conteudo(archiver, indice_membro_extrair);
        long posicao_proximo = encontrar_posicao_conteudo(archiver, indice_membro_extrair + 1);
        tamanho_vazio = posicao_proximo - posicao_atual - membros[indice_membro_extrair].tamanho;
    } else {
        // Último membro
        long posicao_atual = encontrar_posicao_conteudo(archiver, indice_membro_extrair);
        fseek(arquivo_archive, 0, SEEK_END);
        long posicao_fim = ftell(arquivo_archive);
        tamanho_vazio = posicao_fim - posicao_atual - membros[indice_membro_extrair].tamanho;
    }

    // Deslocar os membros restantes
    for (int i = indice_membro_extrair + 1; i < num_membros; i++) {
        membros[i - 1] = membros[i];
    }

    // Preencher o espaço vazio com bytes nulos
    fseek(arquivo_archive, -tamanho_vazio, SEEK_END);
    for (long i = 0; i < tamanho_vazio; i++) {
        fputc(0, arquivo_archive);
    }

    // Atualizar a posição do diretório
    membros[num_membros - 1] = archiver->archiveData.diretorio;

    // Atualizar a estrutura do diretório
    archiver->archiveData.diretorio.num_membros--;

    // Salvar as alterações no arquivo do archive
    salvar_archive(archiver);
}


void extrair_membro(Archiver *archiver, int indice) {
    if (archiver == NULL) {
        printf("Erro: Archiver não inicializado.\n");
        return;
    }

    if (indice < 0 || indice >= archiver->archiveData.diretorio.num_membros) {
        printf("Erro: Índice de membro inválido.\n");
        return;
    }

    Membro *membro = &(archiver->archiveData.diretorio.membros[indice]);

    // Abrir o arquivo do archive
    FILE *arquivo_archive = fopen(archiver->archiveData.nome_arquivo, "rb");
    if (arquivo_archive == NULL) {
        printf("Erro ao abrir o arquivo archive '%s' para leitura.\n", archiver->archiveData.nome_arquivo);
        return;
    }

    // Posicionar o ponteiro do arquivo na posição correta do conteúdo do membro
    long posicao_conteudo = encontrar_posicao_conteudo(archiver, indice);
    if (!navegar_para_posicao(arquivo_archive, posicao_conteudo)) {
        fclose(arquivo_archive);
        return;
    }

    // Criar um novo arquivo para armazenar o membro extraído
    char nome_arquivo_destino[MAX_NOME_ARQUIVO];
    snprintf(nome_arquivo_destino, sizeof(nome_arquivo_destino), "%s%s", "extraidos/", membro->nome_arquivo);
    FILE *arquivo_destino = fopen(nome_arquivo_destino, "wb");
    if (arquivo_destino == NULL) {
        printf("Erro ao criar o arquivo destino '%s'.\n", nome_arquivo_destino);
        fclose(arquivo_archive);
        return;
    }

    // Ler o conteúdo do membro e gravá-lo no arquivo destino
    if (!ler_conteudo_membro(arquivo_archive, membro, arquivo_destino)) {
        fclose(arquivo_archive);
        fclose(arquivo_destino);
        return;
    }

    atualizar_membros(archiver, indice);

    // Fechar os arquivos
    fclose(arquivo_archive);
    fclose(arquivo_destino);

    printf("Membro '%s' extraído com sucesso para o arquivo '%s'.\n", membro->nome_arquivo, nome_arquivo_destino);
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

    archiver->archiveData.diretorio.num_membros = 0;

    memset(archiver->archiveData.diretorio.membros, 0, sizeof(archiver->archiveData.diretorio.membros));

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

    return 0;
}