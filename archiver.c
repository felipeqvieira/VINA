#include "archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

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

void inicializar_archive(Archive * archive){
   
    archive->num_membros = 0;

}

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

/* void adicionar_membro(Archive* archive, const char* nome_membro) {

    FILE* membro = fopen(nome_membro, "rb");

    if (membro == NULL) {
        return ERRO_NA_ABERTURA;
    }

    if (fseek(membro, 0L, SEEK_END) != 0) {
        fclose(membro);
        return ERRO_FSEEK;
    }

    long posicao_atual = ftell(membro);

    if (posicao_atual == -1L) {
        fclose(membro);
        return ERRO_FTELL;
    }

    Membro* novo_membro;

    inicializar_membro(&novo_membro, nome_membro);

    preencher_dados_membro(novo_membro, nome_membro);

    novo_membro->localizacao = posicao_atual;

    novo_membro->ordem = archive->num_membros;

    fwrite(novo_membro, sizeof(Membro), 1, membro);

    archive->membros[archive->num_membros] = novo_membro;

    archive->num_membros++;

    posicao_atual = ftell(membro);

    if (posicao_atual == -1L) {
        fclose(membro);
        return ERRO_FTELL;
    }

    fseek(membro, 0, SEEK_SET);

    fwrite(archive, sizeof(Archive), 1, membro);

    fclose(membro);

}
*/

void adicionar_membro(Archive* archive, const char* nome_membro, const char* nome_arquivo_destino) {
    FILE* membro = fopen(nome_membro, "rb");
    if (membro == NULL) {
        return ERRO_NA_ABERTURA;
    }
    fseek(membro, 0, SEEK_END);
    long tamanho_arquivo = ftell(membro);
    rewind(membro);

    Membro* novo_membro;
    inicializar_membro(&novo_membro, nome_membro);
    preencher_dados_membro(novo_membro, nome_membro);

    FILE* arquivo_destino = fopen(nome_arquivo_destino, "ab");
    if (arquivo_destino == NULL) {
        fclose(membro);
        return ERRO_NA_ABERTURA;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_lidos;

    while ((bytes_lidos = fread(buffer, 1, BUFFER_SIZE, membro)) > 0) {
        // Gravar os dados do buffer em partes de 1024 bytes
        for (size_t i = 0; i < bytes_lidos; i += 1024) {
            size_t tamanho_gravado = bytes_lidos - i;
            if (tamanho_gravado > 1024) {
                tamanho_gravado = 1024;
            }
            // Gravar os dados do buffer no local correto dentro do arquivo de destino
            fwrite(&buffer[i], tamanho_gravado, 1, arquivo_destino);
        }
    }

    archive->membros[archive->num_membros] = novo_membro;
    archive->num_membros++;

    fclose(membro);
    fclose(arquivo_destino);

}

void substituir_membro(Membro* novo_membro, int indice_existente, Membro* membro_existente){

    strcpy(membro_existente->nome, novo_membro->nome);
    
    membro_existente->permissoes = novo_membro->permissoes;
    
    membro_existente->tamanho = novo_membro->tamanho;
    
    membro_existente->data_modificacao = novo_membro->data_modificacao;
    
    membro_existente->ordem = novo_membro->ordem;
    
    membro_existente->localizacao = novo_membro->localizacao;
    
    membro_existente->user_ID = novo_membro->user_ID;
    
}

int encontrar_nome (Archive* archive, const char* nome_membro){

    for(int i = 0; i < archive->num_membros; i++){
        
        if(strcmp(archive->membros[i]->nome, nome_membro) == 0)
            return i;
        
    }

    return -1;
};

/*int adiciona_verificando_data (Archive *archive, const char* nome_membro){

    FILE* arquivo;

    arquivo = fopen(nome_membro, "rb");

    if (arquivo == NULL){
        perror("Erro ao abrir arquivo");
        return ERRO_NA_ABERTURA;
    }

    Membro* novo_membro;

    inicializar_membro(&novo_membro, nome_membro);

    preencher_dados_membro(&novo_membro, nome_membro);

    int indice_existente = encontrar_nome(archive, novo_membro->nome);

    if(indice_existente == -1){
        adicionar_membro(archive, novo_membro->nome);
    } else {
        Membro* membro_existente = archive->membros[indice_existente];

        if(novo_membro->data_modificacao > membro_existente->data_modificacao){
            substituir_membro(novo_membro, indice_existente, membro_existente);
        }

    }

    fclose(arquivo);

    return ARQUIVO_VERIFICADO;

};
*/

int adiciona_verificando_data(Archive* archive, const char* nome_membro, const char* arquivo_destino) {
    FILE* arquivo = fopen(nome_membro, "rb");
    if (arquivo == NULL) {
        perror("Erro ao abrir arquivo");
        return ERRO_NA_ABERTURA;
    }

    Membro* novo_membro;
    inicializar_membro(&novo_membro, nome_membro);
    preencher_dados_membro(novo_membro, nome_membro);

    int indice_existente = encontrar_nome(archive, novo_membro->nome);

    if (indice_existente == -1) {
        adicionar_membro(archive, novo_membro->nome, arquivo_destino);
    } else {
        Membro* membro_existente = archive->membros[indice_existente];

        if (novo_membro->data_modificacao > membro_existente->data_modificacao) {
            substituir_membro(novo_membro, indice_existente, membro_existente);
        }
    }

    fclose(arquivo);

    return ARQUIVO_VERIFICADO;
}

void atualizar_diretorio(Archive* archive, FILE* arquivo) {
    fseek(arquivo, sizeof(int) + sizeof(Membro) * archive->num_membros, SEEK_SET);

    for (int i = 0; i < archive->num_membros; i++) {
        
        Membro* membro = archive->membros[i];

        fseek(arquivo, sizeof(int) + sizeof(Membro) * i, SEEK_SET);

        fwrite(membro, sizeof(Membro), 1, arquivo);
        
    }
}

void escrever_arquivo_archive(Archive* archive) {

    FILE* arquivo = fopen("archive.vpp", "wb");

    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo 'archive.vpp' para escrita");
        return ERRO_NA_ABERTURA;
    }

    fseek(arquivo, 0, SEEK_SET);

    fwrite(archive, sizeof(Archive), 1, arquivo);
    
    fclose(arquivo);
}

int mover_membro(Archive *archive, const char* membro_frente, const char* membro_atras){

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

    escrever_arquivo_archive(archive);

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