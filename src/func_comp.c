#include "archiver.h"

char *strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup != NULL) {
        memcpy(dup, s, len);
    }
    return dup;
}

/* verifica se um membro faz parte do archive */
int verifica_nome(Archiver *archive, const char *nome_arquivo) {
    for (int i = 0; i < archive->diretorio.num_membros; i++) {
        if (strcmp(nome_arquivo, archive->diretorio.membros[i].nome) == 0) {
            return i;  // O nome do arquivo já existe no Archive
        }
    }
    return -1;  // O nome do arquivo não existe no Archive
}

/* conta os membros passados por argumento */
int contar_membros(char **membros) {
    int contador = 0;
    while (membros[contador] != NULL) {
        contador++;
    }
    return contador;
}

/* extrai as propriedades do membro */
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
    membro->ordem = archiver->diretorio.num_membros; 
    membro->localizacao = archiver->diretorio.num_membros; 
}

/* verifica a existencia do archive, caso exista ele carrega o diretorio, no contrario ele inicializa um archive */
void verificar_archive_existente(Archiver *archiver) {

    FILE *arquivo = fopen(archiver->nome_archive, "r+b");
    if (arquivo == NULL) {
        printf("O arquivo '%s' nao existe. Um novo arquivo sera criado.\n", archiver->nome_archive);

        // Cria um novo arquivo vazio
        arquivo = fopen(archiver->nome_archive, "w+b");
        if (arquivo == NULL) {
            printf("Erro ao criar o arquivo '%s'.\n", archiver->nome_archive);
            return;
        }

        // Salva a estrutura do diretório no arquivo
        fwrite(&(archiver->diretorio), sizeof(Diretorio), 1, arquivo);

        fclose(arquivo);

        return;
    }

    fseek(arquivo, -sizeof(Diretorio), SEEK_END);
    fread(&(archiver->diretorio), sizeof(Diretorio), 1, arquivo);
    fclose(arquivo);

}

/* cria os diretorios para que se extraia um membro no local certo */
void criar_diretorio(const char *caminho) {
    char *caminho_temp = strdup(caminho);
    char *diretorio = strtok(caminho_temp, "/");
    char *diretorio_atual = NULL;

    while (diretorio != NULL) {
        if (diretorio_atual == NULL) {
            diretorio_atual = strdup(diretorio);
        } else {
            diretorio_atual = realloc(diretorio_atual, strlen(diretorio_atual) + strlen(diretorio) + 2);
            strcat(diretorio_atual, "/");
            strcat(diretorio_atual, diretorio);
        }

        // Verifica se o diretório existe
        struct stat info;
        if (stat(diretorio_atual, &info) == -1) {
            // O diretório não existe, cria-o
            if (mkdir(diretorio_atual, 0777) == -1) {
                perror("Erro ao criar diretório");
                free(diretorio_atual);
                free(caminho_temp);
                return;
            }
        }

        diretorio = strtok(NULL, "/");
    }

    free(diretorio_atual);
    free(caminho_temp);
}

/* atualiza a localizacao dos membros depois de uma remoção */
void atualizar_localizacao_membros(Archiver *archiver) {
    long deslocamento = 0;

    for (int j = 0; j < archiver->diretorio.num_membros; j++) {
        
        archiver->diretorio.membros[j].localizacao = deslocamento;
        deslocamento += archiver->diretorio.membros[j].tamanho;

    }
}
