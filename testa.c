#include "archiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME_ARQUIVO 1024
#define MAX_CAMINHO_ARQUIVO 1024

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
    printf("1. Inicializar Archive\n");
    printf("2. Adicionar Membro\n");
    printf("3. Substituir Membro\n");
    printf("4. Encontrar Nome\n");
    printf("5. Adicionar Verificando Data\n");
    printf("6. Atualizar Diretório\n");
    printf("7. Escrever Arquivo Archive\n");
    printf("8. Mover Membro\n");
    printf("9. Extrair Membros\n");
    printf("10. Listar Conteúdo\n");
    printf("11. Remover Membro\n");
    printf("12. Exibir Ajuda\n");
    printf("0. Sair\n");
    printf("Opção: ");
    scanf("%d", &opcao);

    switch (opcao) {
        case 1:
            adicionar_membro(archive, argv[3], argv[2]);
            break;
        /*
        case 2:
            adicionar_membro(archive, argv[3], argv[2]);
            break;
        case 3:
            
            break;
        case 4:
           
            break;
        case 5:
            
            break;
        case 6:
            
            break;
        case 7:
            
            break;
        case 8:
            
            break;
        case 9:
            
            break;
        case 10:
           
            break;
        case 11:
            
            break;
        case 12:
            exibir_ajuda();
            break;
        case 0:
            
            break;
            */
        default:
            printf("Opção inválida.\n");
            break;
    }

    return 0;
}