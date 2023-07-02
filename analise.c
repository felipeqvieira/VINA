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

void armazenar_caminho_relativo(Membro *membro) {
    char *nome = membro->nome;

    if (nome[0] != '/') {
        // O nome não é um caminho absoluto, não é necessário converter
        return;
    }

    char *diretorio_atual = getcwd(NULL, 0);
    int tamanho_diretorio_atual = strlen(diretorio_atual);

    char *caminho_relativo = malloc(strlen(nome) + 2 + tamanho_diretorio_atual);
    if (caminho_relativo == NULL) {
        printf("Erro ao alocar memória.\n");
        return;
    }

    strcpy(caminho_relativo, ".");
    strcat(caminho_relativo, nome + 1);  // Ignora a barra inicial do caminho absoluto

    strcpy(membro->nome, caminho_relativo);

    free(caminho_relativo);
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

}












void exibir_ajuda() {
    printf("Opções disponíveis:\n");
    printf("-i : insere/acrescenta um ou mais membros ao archive. Caso o membro já exista no archive, ele deve ser substituído. Novos membros são inseridos respeitando a ordem da linha de comando, ao final do archive.\n");
    printf("-a : mesmo comportamento da opção -i, mas a substituição de um membro existente ocorre APENAS caso o parâmetro seja mais recente que o arquivado.\n");
    printf("-m target : move o membro indicado na linha de comando para imediatamente depois do membro target existente em archive. A movimentação deve ocorrer na seção de dados do archive.\n");
    printf("-x : extrai os membros indicados de archive. Se os membros não forem indicados, todos devem ser extraídos. A extração consiste em ler o membro de archive e criar um arquivo correspondente, com conteúdo idêntico, em disco.\n");
    printf("-r : remove os membros indicados de archive.\n");
    printf("-c : lista o conteúdo de archive em ordem, incluindo as propriedades de cada membro (nome, UID, permissões, tamanho e data de modificação) e sua ordem no arquivo. A saída esperada é igual ao do comando tar com as opções tvf.\n");
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
                membro_existente = 1;
                break;
            }
        }
        if (membro_existente) {
            continue;  // Passa para o próximo arquivo
        }

        // Verifica se há espaço disponível no diretório
        if (archiver->archiveData.diretorio.num_membros >= QUANTIDADE_MAXIMA) {
            printf("Não há espaço disponível para adicionar o membro '%s'.\n", nome_membro);
            continue;  // Passa para o próximo arquivo
        }

        // Cria uma nova estrutura de Membro para armazenar as informações
        Membro novo_membro;
        extrair_informacoes_membro(archiver, nome_membro, &novo_membro);

        // Abre o arquivo do membro em modo de leitura binária
        FILE *arquivo_membro = fopen(nome_membro, "rb");
        if (arquivo_membro == NULL) {
            printf("Erro ao abrir o arquivo do membro '%s'.\n", nome_membro);
            continue;  // Passa para o próximo arquivo
        }

        long deslocamento = 0;
        for (int j = 0; j < archiver->archiveData.diretorio.num_membros; j++) {
            deslocamento += archiver->archiveData.diretorio.membros[j].tamanho;
        }

        novo_membro.localizacao = deslocamento;

        // Move o cursor para o início do conteúdo do membro
        fseek(arquivo_archive, deslocamento, SEEK_SET);

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
    /* fseek(arquivo_archive, sizeof(ArchiveData), SEEK_SET);*/
    fseek(arquivo_archive, 0, SEEK_END);

    // Escreve o novo diretório no arquivo
    fwrite(&(archiver->archiveData.diretorio), sizeof(Diretorio), 1, arquivo_archive);

    // Fecha o arquivo do archive
    fclose(arquivo_archive);
}




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
            if ((dimkdirretorio_atual) == -1) {
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

        // Move o cursor para o início do conteúdo do membro
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

void atualizar_localizacao_membros(Archiver *archiver) {
    long deslocamento = 0;

    for (int j = 0; j < archiver->archiveData.diretorio.num_membros; j++) {
        
        archiver->archiveData.diretorio.membros[j].localizacao = deslocamento;
        deslocamento += archiver->archiveData.diretorio.membros[j].tamanho;

    }
}


void remover_membros(Archiver *archiver, char *nome_archive, char **nomes_membros, int num_membros) {

    // Abre o arquivo original do archive para leitura
    FILE *arquivo_original = fopen(nome_archive, "rb");
    if (arquivo_original == NULL) {
        printf("Erro ao abrir o arquivo original do archive.\n");
        return;
    }

    // Cria o arquivo temporário do archive para escrita
    FILE *arquivo_temporario = fopen("temp_archive", "wb");
    if (arquivo_temporario == NULL) {
        printf("Erro ao criar o arquivo temporário do archive.\n");
        fclose(arquivo_original);
        return;
    }

    // Atualiza a estrutura do diretório removendo os membros desejados e reorganizando os índices
    int num_membros_atualizado = archiver->archiveData.diretorio.num_membros;
    int num_membros_removidos = 0;

    for (int i = 0; i < archiver->archiveData.diretorio.num_membros; i++) {
        Membro *membro = &(archiver->archiveData.diretorio.membros[i]);

        int membro_removido = 0;
        for (int j = 0; j < num_membros; j++) {
            if (strcmp(membro->nome, nomes_membros[j]) == 0) {
                membro_removido = 1;
                num_membros_atualizado--;
                num_membros_removidos++;
                break;
            }
        }

        if (!membro_removido) {
            if (num_membros_removidos > 0) {
                // Move o membro para a posição atualizada
                archiver->archiveData.diretorio.membros[i - num_membros_removidos] = *membro;
            }
        }
    }

    archiver->archiveData.diretorio.num_membros = num_membros_atualizado;

    // Passa os membros do diretório em ordem, escrevendo no arquivo temporário os membros que não foram escolhidos para serem removidos
    for (int i = 0; i < num_membros_atualizado; i++) {
        Membro *membro = &(archiver->archiveData.diretorio.membros[i]);

        long deslocamento = membro->localizacao;

        fseek(arquivo_original, deslocamento, SEEK_SET);

        long tamanho_membro = membro->tamanho;

        unsigned char buffer[MAX_CONTEUDO];

        int quociente = tamanho_membro / MAX_CONTEUDO;
 
        for (int j = 0; j < quociente; j++) {
            size_t bytes_lidos = fread(buffer, sizeof(unsigned char), MAX_CONTEUDO, arquivo_original);

            if (bytes_lidos == 0) {
                printf("Erro ao ler o conteúdo do archive.\n");
                fclose(arquivo_temporario);
                remove("temp_archive");
                break;
            }

            size_t bytes_gravados = fwrite(buffer, sizeof(unsigned char), bytes_lidos, arquivo_temporario);

            if (bytes_gravados < bytes_lidos) {
                printf("Erro ao gravar o conteúdo no arquivo extraído.\n");
                fclose(arquivo_temporario);
                remove("temp_archive");
                break;
            }
        }
    

        int resto = tamanho_membro % MAX_CONTEUDO;

        if (resto > 0) {
            size_t bytes_lidos = fread(buffer, sizeof(unsigned char), resto, arquivo_original);

            if (bytes_lidos == 0) {
                printf("Erro ao ler o conteúdo do arquivo.\n");
                fclose(arquivo_temporario);
                remove("temp_archive");
                continue;
            }

            size_t bytes_gravados = fwrite(buffer, sizeof(unsigned char), bytes_lidos, arquivo_temporario);

            if (bytes_gravados < bytes_lidos) {
                printf("Erro ao gravar o conteúdo no arquivo extraído.\n");
                fclose(arquivo_temporario);
                remove("temp_archive");
                continue;
            }
        }
    }

    fseek(arquivo_temporario, 0, SEEK_END);

    // Escreve o diretório atualizado no final do arquivo temporário do archive
    if (fwrite(&(archiver->archiveData.diretorio), sizeof(Diretorio), 1, arquivo_temporario) != 1) {
        printf("Erro ao escrever o diretório atualizado no arquivo temporário do archive.\n");
        fclose(arquivo_original);
        fclose(arquivo_temporario);
        remove("temp_archive");
        return;
    }

    // Fecha os arquivos
    fclose(arquivo_original);
    fclose(arquivo_temporario);

    // Renomeia o arquivo temporário para substituir o arquivo original do archive
     if (remove(nome_archive) != 0) {
        printf("Erro ao remover o arquivo original do archive.\n");
        remove("temp_archive");
        return;
    }

    if (rename("temp_archive", nome_archive) != 0) {
        printf("Erro ao renomear o arquivo temporário para substituir o arquivo original do archive.\n");
        remove("temp_archive");
        return;
    }

    atualizar_localizacao_membros(archiver);
    
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
    } else if (strcmp(opcao, "-x") == 0) {
        extrair_membro(archiver, archive, membros, num_membros);
    }else if (strcmp(opcao, "-c") == 0) {
        listar_membros(archiver);
    } else if (strcmp(opcao, "-r") == 0) {
        remover_membros(archiver, archive, membros, num_membros);
    } else if (strcmp(opcao, "-h") == 0) {
        exibir_ajuda();
    } else {
        printf("Opção inválida.\n");
        exibir_ajuda();
    }

    free(archiver);

    return 0;
}