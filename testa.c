#include "archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME_ARQUIVO 1024
#define MAX_CAMINHO_ARQUIVO 1024

void escrever_arquivo_archive(Archive* archive, const char* nome_archive) {

    FILE* arquivo = fopen(nome_archive, "wb");

    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return ERRO_NA_ABERTURA;
    }

    fseek(arquivo, 0, SEEK_SET);

    fwrite(archive, sizeof(Archive), 1, arquivo);
    
    fclose(arquivo);
}

void atualizar_diretorio(Archive* archive, FILE* arquivo) {
    fseek(arquivo, sizeof(int) + sizeof(Membro) * archive->num_membros, SEEK_SET);

    for (int i = 0; i < archive->num_membros; i++) {
        
        Membro* membro = archive->membros[i];

        fseek(arquivo, sizeof(int) + sizeof(Membro) * i, SEEK_SET);

        fwrite(membro, sizeof(Membro), 1, arquivo);
        
    }
}

int encontrar_nome (Archive* archive, const char* nome_membro){

    for(int i = 0; i < archive->num_membros; i++){
        
        if(strcmp(archive->membros[i]->nome, nome_membro) == 0)
            return i;
        
    }

    return -1;
};

void preencher_dados_membro(Membro* membro, const char* nome_arquivo){

    struct stat file_info;

    if (lstat(nome_arquivo, &file_info) == -1) {
        perror("Erro ao obter informações do arquivo");
        return;
    }

    membro->tamanho = file_info.st_size;

    membro->data_modificacao = file_info.st_mtime;

    // Obtendo o user_ID
    membro->user_ID = file_info.st_uid;

    // Obtendo as permissões
    membro->permissoes = (file_info.st_mode & S_IRWXU) | 0070 | 0007;

}

void exibir_ajuda() {
    printf("Opções disponíveis:\n");
    printf("-i : insere/acrescenta um ou mais membros ao archive. Caso o membro já exista no archive, ele deve ser substituído. Novos membros são inseridos respeitando a ordem da linha de comando, ao final do archive;\n");
    printf("-a : mesmo comportamento da opção -i, mas a substituição de um membro existente ocorre APENAS caso o parâmetro seja mais recente que o arquivado;\n");
    printf("-m target : move o membro indicado na linha de comando para imediatamente depois do membro target existente em archive. A movimentação deve ocorrer na seção de dados do archive;\n");
    printf("-x : extrai os membros indicados de archive. Se os membros não forem indicados, todos devem ser extraídos. A extração consiste em ler o membro de archive e criar um arquivo correspondente, com conteúdo idêntico, em disco;\n");
    printf("-r : remove os membros indicados de archive;\n");
    printf("-c : lista o conteúdo de archive em ordem, incluindo as propriedades de cada membro (nome, UID, permissões, tamanho e data de modificação) e sua ordem no arquivo. A saída esperada é igual ao do comando tar com as opções tvf\n");
    printf("-h : imprime uma pequena mensagem de ajuda com as opções disponíveis e encerra.\n");
}

Archive * inicializar_archive(const char* nome_arquivo){
   
    Archive* novo_archive = (Archive*)malloc(sizeof(Archive));

    if(novo_archive == NULL){
        perror("Erro ao alocar memória para o Archive");
        return NULL;
    }

    strcpy(novo_archive->nome_archive, nome_arquivo);
    novo_archive->num_membros = 0;

    return novo_archive;
}

void inicializar_membro(Membro** membro, const char* nome) {
    
    *membro = malloc(sizeof(Membro));

    strncpy((*membro)->nome, nome, sizeof((*membro)->nome));

    (*membro)->nome[sizeof((*membro)->nome) - 1] = '\0';
    (*membro)->user_ID = 0;
    (*membro)->permissoes = 0;
    (*membro)->tamanho = 0;
    (*membro)->data_modificacao = 0;
    (*membro)->ordem = 0;
    (*membro)->localizacao = 0;
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

    while ((bytes_lidos = fread(buffer, 1, BUFFER_SIZE, membro)) > 0) {
        // Gravar os dados do buffer no arquivo de destino
        fwrite(buffer, 1, bytes_lidos, arquivo_destino);
    }

    fclose(membro);
    fclose(arquivo_destino);
}

void substituir_membro(Archive* archive, int indice_membro, const char* nome_novo_membro, const char* nome_arquivo_destino) {
    // Verificar se o índice do membro está dentro dos limites válidos
    if (indice_membro < 0 || indice_membro >= archive->num_membros) {
        printf("Índice de membro inválido\n");
        return;
    }

    // Abrir o novo arquivo de membro em modo de leitura binária
    FILE* novo_membro = fopen(nome_novo_membro, "rb");
    if (novo_membro == NULL) {
        perror("Erro ao abrir o novo arquivo de membro");
        return;
    }

    // Obter o tamanho do novo membro
    fseek(novo_membro, 0, SEEK_END);
    long tamanho_novo_membro = ftell(novo_membro);
    rewind(novo_membro);

    // Alocar memória para o novo membro
    Membro* membro_substituto = (Membro*)malloc(sizeof(Membro));
    if (membro_substituto == NULL) {
        perror("Erro ao alocar memória para o novo membro");
        fclose(novo_membro);
        return;
    }

    // Preencher as informações do novo membro
    strncpy(membro_substituto->nome, nome_novo_membro, MAX_NOME_ARQUIVO);
    membro_substituto->tamanho = tamanho_novo_membro;
    membro_substituto->permissoes = obter_permissoes(nome_novo_membro);
    membro_substituto->data_modificacao = obter_data_modificacao(nome_novo_membro);

    // Substituir o membro existente pelo novo membro
    Membro* membro_existente = archive->membros[indice_membro];
    free(membro_existente);
    archive->membros[indice_membro] = membro_substituto;

    // Atualizar o diretório do arquivo
    atualizar_diretorio(archive->nome_archive, archive);

    // Abrir o arquivo de destino em modo de escrita binária
    FILE* arquivo_destino = fopen(archive->nome_archive, "r+b");
    if (arquivo_destino == NULL) {
        perror("Erro ao abrir o arquivo de destino");
        fclose(novo_membro);
        return;
    }

    // Deslocar o ponteiro para a posição correta do membro no arquivo de destino
    fseek(arquivo_destino, sizeof(Archiver) + indice_membro * sizeof(Membro*), SEEK_SET);

    // Gravar o ponteiro para o novo membro no arquivo de destino
    fwrite(&membro_substituto, sizeof(Membro*), 1, arquivo_destino);

    // Fechar os arquivos
    fclose(novo_membro);
    fclose(arquivo_destino);
}

void adicionar_verificando_data(Archive* archive, const char* nome_membro, const char* nome_arquivo_destino) {
    int indice_existente = encontrar_nome(archive, nome_membro);
    if (indice_existente != -1) {
        Membro* membro_existente = archive->membros[indice_existente];
        struct stat info_existente;
        if (stat(membro_existente->nome, &info_existente) != 0) {
            perror("Erro ao obter informações do membro existente");
            return;
        }

        struct stat info_novo;
        if (stat(nome_membro, &info_novo) != 0) {
            perror("Erro ao obter informações do novo membro");
            return;
        }

        if (info_novo.st_mtime > info_existente.st_mtime || info_novo.st_mtime == info_existente.st_mtime) {
            substituir_membro(archive, indice_existente, nome_membro, nome_arquivo_destino);
        }
    } else {
        adicionar_membro(archive, nome_membro, nome_arquivo_destino);
    }
}

int mover_membro(Archive *archive, const char *nome_archive, const char* membro_frente, const char* membro_atras){

    FILE* arquivo_destino = fopen(membro_atras,"rb");

    if (arquivo_destino == NULL){
        perror("Erro ao abrir arquivo destino");
        fclose(arquivo_destino);
        return ERRO_NA_ABERTURA;
    }

    int indice1 = encontrar_nome(archive, membro_atras);


    FILE* arquivo_movido = fopen(membro_frente,"rb");

    if (arquivo_movido == NULL){
        perror("Erro ao abrir arquivo movido");
        fclose(arquivo_movido);
        fclose(arquivo_destino);
        return ERRO_NA_ABERTURA;
    }

    int indice2 = encontrar_nome(archive, membro_frente);

    if(indice2 == indice1 + 1){
        fclose(arquivo_destino);
        fclose(arquivo_movido);
        return ARQUIVO_VERIFICADO;
    }

    Membro* membro_temp = archive->membros[indice2];

    for(int i = indice2; i < indice1; i++){
        archive->membros[i] = archive->membros[i+1];
    }

    archive->membros[indice1] = membro_temp;

    atualizar_diretorio(archive,arquivo_destino);

    escrever_arquivo_archive(archive, nome_archive);

    fclose(arquivo_destino);
    fclose(arquivo_movido);

    return ARQUIVO_VERIFICADO;

}

void extrair_membros(Archive *archive, const char* nome_membro){

    int indice_membro = encontrar_nome(archive,nome_membro);

    if (indice_membro == -1){
        perror("Membro nao foi encontrado no 'archive'.\n");
        return ARQUIVO_NAO_ENCONTRADO;
    }

    FILE* arquivo_extraido = fopen(nome_membro,"wb");
    if  (arquivo_extraido == NULL){
        perror("Erro ao criar arquivo.\n");
        return ERRO_NA_ABERTURA;
    }

    Membro* membro = archive->membros[indice_membro];

    fwrite(membro,sizeof(Membro),1,arquivo_extraido);

    fclose(arquivo_extraido);

}

void listar_conteudo(const char* nome_arquivo) {
    FILE* arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para leitura");
        return;
    }

    Archiver archiver;
    fread(&archiver, sizeof(Archiver), 1, arquivo);

    Archive* archive = &(archiver.archive);
    
    printf("Conteúdo do arquivo '%s':\n", nome_arquivo);

    for (int i = 0; i < archive->num_membros; i++) {
        Membro* membro = archive->membros[i];
        printf("Nome: %s\n", membro->nome);
        printf("Tamanho: %ld bytes\n", membro->tamanho);
        printf("Propriedades: %d\n", membro->permissoes);
        printf("---------------\n");
    }

    fclose(arquivo);
}

void remover_membro(const char* nome_arquivo, const char* nome_membro) {
    FILE* arquivo = fopen(nome_arquivo, "r+b");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para leitura/escrita");
        return;
    }

    Archiver archiver;
    fread(&archiver, sizeof(Archiver), 1, arquivo);

    Archive* archive = &(archiver.archive);

    // Localizar o membro indicado para remoção
    int indice_membro = -1;
    for (int i = 0; i < archive->num_membros; i++) {
        Membro* membro = archive->membros[i];
        if (strcmp(membro->nome, nome_membro) == 0) {
            indice_membro = i;
            break;
        }
    }

    if (indice_membro == -1) {
        printf("O membro '%s' não foi encontrado.\n", nome_membro);
        fclose(arquivo);
        return;
    }

    // Remover o membro especificado do "archive"
    free(archive->membros[indice_membro]);
    archive->num_membros--;

    // Atualizar as informações do diretório do "archive" após a remoção

    // Deslocar os membros restantes para preencher o espaço vazio
    for (int i = indice_membro; i < archive->num_membros; i++) {
        archive->membros[i] = archive->membros[i + 1];
    }

    // Truncar o arquivo para remover o último membro, que agora está duplicado no final
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&archiver, sizeof(Archiver), 1, arquivo);
    fflush(arquivo);
    ftruncate(fileno(arquivo), sizeof(Archiver) + archive->num_membros * sizeof(Membro*));

    fclose(arquivo);

    printf("O membro '%s' foi removido com sucesso.\n", nome_membro);
}

// Defina as estruturas e as funções aqui

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("É necessário fornecer o nome do arquivo como argumento.\n");
        return 1;
    }

    char *nome_arquivo = argv[1];

    // Outras variáveis necessárias
    int opcao;
    Archive* archive;

    archive = inicializar_archive(nome_arquivo);

    size_t size = sizeof(**argv) / sizeof(*argv[0]);
  
    printf("The length of the array is %d \n", size);

    // Opções do usuário
    printf("Selecione uma opção:\n");
    printf("1. Adicionar Membro\n");
    printf("2. Adicionar Verificando Data\n");
    printf("3. Mover Membro\n");
    printf("4. Extrair Membros\n");
    printf("5. Listar Conteúdo\n");
    printf("6. Remover Membro\n");
    printf("7. Exibir Ajuda\n");
    printf("0. Sair\n");
    printf("Opção: ");
    scanf("%d", &opcao);


    switch (opcao) {
        case 1:
            adicionar_membro(archive, argv[3], argv[2]);
            break;
        case 2:
            adiciona_verificando_data(archive, argv[3], argv[2]);
            break;
        case 3:
            mover_membro(archive, argv[2], argv[3], argv[4]);
            break;
        case 4:
            extrair_membros(archive, argv[2]);
            break;
        case 5:
            listar_conteudo(argv[2]);
            break;
        case 6:
            remover_membro(argv[2], argv[3]);
            break;
        case 7:
            exibir_ajuda();
            break;
        case 0:
            return 1;
            break;
        default:
            printf("Opção inválida.\n");
            break;
    }

    printf("Opção: ");
    scanf("%d", &opcao);

}