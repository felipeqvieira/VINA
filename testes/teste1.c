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
#define BUFFER_SIZE 1024

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

int contar_membros(char **membros) {
    int contador = 0;
    while (membros[contador] != NULL) {
        contador++;
    }
    return contador;
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

void inserir_membros(Archiver *archiver, const char *nome_membro) {

    // Verifica se o arquivo já existe no ArchiveData
    if (verifica_nome(&(archiver->archiveData), nome_membro)) {
        printf("O membro '%s' já existe no ArchiveData.\n", nome_membro);
        return;
    }

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

void inserir_membros_diversos(Archiver *archiver, char **membros, int num_membros) {

    for (int i = 0; i < num_membros; i++) {

        const char *nome_membro = membros[i];

        // Verifica se o arquivo já existe no ArchiveData
        if (verifica_nome(&(archiver->archiveData), nome_membro)) {
            printf("O membro '%s' já existe no ArchiveData.\n", nome_membro);
            return;
        }

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
        if (verifica_nome(&(archiver->archiveData), membros[i])) {
            printf("O membro '%s' já existe no ArchiveData.\n", membros[i]);
            continue;
        }
    }
}

void inserir_data(Archiver *archiver, char **membros, int num_membros) {
    
    // Verificar se o archiver existe
    if (verifica_nome(&(archiver->archiveData), archiver->archiveData.nome_archive)) {
        printf("O archiver '%s' já existe.\n", archiver->archiveData.nome_archive);
        return;
    }

    // Verificar se o membro existe no archiver
    for (int i = 0; i < num_membros; i++) {
        const char *nome_membro = membros[i];

        if (verifica_nome(&(archiver->archiveData), nome_membro)) {
            int posicao_existente = -1;

            for (int j = 0; j < archiver->archiveData.diretorio.num_membros; j++) {
                if (strcmp(nome_membro, archiver->archiveData.diretorio.membros[j].nome) == 0) {
                    posicao_existente = j;
                    break;
                }
            }

            if (posicao_existente != -1) {
                Membro novo_membro;
                extrair_informacoes_membro(nome_membro, &novo_membro);

                // Comparar datas
                if ( verificar_tempo_modificacao_mais_recente(nome_membro, &(archiver->archiveData.diretorio.membros[posicao_existente]))) {
                    FILE *arquivo_membro = fopen(nome_membro, "rb");
                    if (arquivo_membro == NULL) {
                        printf("Erro ao abrir o arquivo do membro '%s'.\n", nome_membro);
                        continue;
                    }

                    // Lê o conteúdo do membro
                    size_t bytes_lidos = fread(archiver->archiveData.conteudo.conteudo, sizeof(unsigned char), MAX_CONTEUDO, arquivo_membro);
                    fclose(arquivo_membro);

                    if (bytes_lidos == 0) {
                        printf("Erro ao ler o conteúdo do membro '%s'.\n", nome_membro);
                        continue;
                    }

                    // Substituir o membro existente apenas se o parâmetro for mais recente
                    substituir_recente(archiver, posicao_existente, &novo_membro, archiver->archiveData.conteudo.conteudo, bytes_lidos);
                } else {
                    printf("O membro '%s' não é mais recente que o membro arquivado.\n", nome_membro);
                }
            }
        } else {
            // Membro não existe, então é inserido normalmente
            inserir_membros(archiver, nome_membro);
        }
    }
}

void extrair_membros(Archiver* archiver, char** membros, int num_membros) {

    printf("num_membros = %d\n", num_membros);

    // Verificar se o arquivo "archive" existe e pode ser aberto
    FILE* arquivo = fopen(archiver->archiveData.nome_archive, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de archive.\n");
        return;
    }

    // Criar diretório para a extração dos membros
    char diretorio_extraido[MAX_NOME_ARCHIVE];
    snprintf(diretorio_extraido, MAX_NOME_ARCHIVE, "%s_extraido", archiver->archiveData.nome_archive);
    if (mkdir(diretorio_extraido) != 0) {
        printf("Erro ao criar o diretório de extração.\n");
        fclose(arquivo);
        return;
    }

    // Verificar se todos os membros devem ser extraídos
    int extrair_todos = (num_membros == 0);

    printf("extrair_todos = %d\n", extrair_todos);

    printf("archiver->archiveData.diretorio.membros[i] = %d\n", archiver->archiveData.diretorio.num_membros);

    // Ler os membros indicados para extração
    for (int i = 0; i < archiver->archiveData.diretorio.num_membros; i++) {


        Membro* membro = &(archiver->archiveData.diretorio.membros[i]);

        // Verificar se o membro deve ser extraído
        int extrair = extrair_todos;

        printf("\nextrair = %d e num_membros = %d\n", extrair, num_membros);
        printf("!extrair = %d\n", !extrair);

        if (!extrair && num_membros > 0) {
            for (int j = 0; j < num_membros; j++) {
                if (strcmp(membros[j], membro->nome) == 0) {
                    extrair = 1;
                    break;
                }
            }
        }

        if (extrair) {
            // Criar o caminho completo para o arquivo extraído
            char caminho_arquivo_extraido[MAX_NOME_ARCHIVE + MAX_NOME];
            snprintf(caminho_arquivo_extraido, MAX_NOME_ARCHIVE + MAX_NOME, "%s/%s", diretorio_extraido, membro->nome);

            // Criar o arquivo extraído
            FILE* arquivo_extraido = fopen(caminho_arquivo_extraido, "wb");
            if (arquivo_extraido == NULL) {
                printf("Erro ao criar o arquivo extraído: %s\n", membro->nome);
                continue;
            }

            // Escrever o conteúdo do membro no arquivo extraído
            fwrite(archiver->archiveData.conteudo.conteudo + membro->localizacao, sizeof(unsigned char), membro->tamanho, arquivo_extraido);

            // Fechar o arquivo extraído
            fclose(arquivo_extraido);

            printf("Membro extraído: %s\n", membro->nome);
        }
    }

    // Fechar o arquivo de archive
    fclose(arquivo);
}

int main(int argc, char *argv[]) {
    int continuar = 1;

    char *archive = argv[2];

    Archiver *archiver = malloc(sizeof(Archiver));
        if (archiver == NULL) {
            printf("Erro ao alocar memória para o Archiver.\n");
            return 1;
        }

    ArchiveData *archiveData = inicializar_archive(archive);
        if (archiveData == NULL) {
            printf("Erro ao inicializar o ArchiveData.\n");
            free(archiver);
            return 1;
        }

    archiver->archiveData = *archiveData;

    while (continuar) {
        // Verificar se foram fornecidos argumentos suficientes
        if (argc < 3) {
            printf("Uso: vina++ <opção> <archive> [membro1 membro2 ...]\n");
            return 1;
        }

        char *opcao = argv[1];
        
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

        int num_membros = contar_membros(membros);

        // Encaminhar para a função apropriada de acordo com a opção
        if (strcmp(opcao, "-i") == 0) {

            if (num_membros > 1)
                inserir_membros_diversos(archiver, membros, num_membros);
            else {
                inserir_membros(archiver, argv[3]);
                printf("%s\n", argv[3]);
            }

        } else if (strcmp(opcao, "-a") == 0) {
            inserir_data(archiver, membros, num_membros);

        } else if (strcmp(opcao, "-m") == 0) {
            /*mover_membro(archive, membros);*/
            printf("oi\n");

        } else if (strcmp(opcao, "-x") == 0) {
            extrair_membros(archiver, membros, num_membros);

        } /*else if (strcmp(opcao, "-r") == 0) {
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

        // Solicita novos argumentos
        printf("Digite os argumentos (ou 0 para sair): ");
        char linha_argumentos[1000];
        fgets(linha_argumentos, sizeof(linha_argumentos), stdin);
        linha_argumentos[strcspn(linha_argumentos, "\n")] = '\0';  // Remove a quebra de linha

        if (strcmp(linha_argumentos, "0") == 0) {
            continuar = 0;  // Sai do loop e encerra o programa
        } else {
            // Atualiza os argumentos para a próxima iteração
            argc = 0;
            argv[argc++] = strtok(linha_argumentos, " ");

            char *token;
            while ((token = strtok(NULL, " ")) != NULL) {
                argv[argc++] = token;
            }
        }
    }

    // Libera a memória alocada
        free(archiveData);
        free(archiver);

    return 0;
}

