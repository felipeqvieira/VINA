#include "archiver.h"

void inserir_membros(Archive *archive, const char *nome_membro) {
    // Verifica se o arquivo já existe no Archive
    if (verifica_nome(archive, nome_membro)) {
        printf("O membro '%s' já existe no Archive.\n", nome_membro);
        return;
    }

    // Verifica se há espaço disponível no diretório
    if (archive->diretorio.num_membros >= QUANTIDADE_MAXIMA) {
        printf("Não há espaço disponível para adicionar o membro.\n");
        return;
    }

    // Cria uma nova estrutura de Membro para armazenar as informações
    Membro novo_membro;
    extrair_informacoes_membro(nome_membro, &novo_membro);

    // Insere o novo membro na próxima posição disponível
    int posicao_insercao = archive->diretorio.num_membros;
    Membro *membro = &archive->diretorio.membros[posicao_insercao];
    memcpy(membro, &novo_membro, sizeof(Membro));

    // Atualiza o número de membros no diretório
    archive->diretorio.num_membros++;

    // Abre o arquivo do membro em modo de leitura binária
    FILE *arquivo_membro = fopen(nome_membro, "rb");
    if (arquivo_membro == NULL) {
        printf("Erro ao abrir o arquivo do membro '%s'.\n", nome_membro);
        return;
    }

    // Lê o conteúdo do membro e o armazena na estrutura ConteudoMembro
    size_t bytes_lidos = fread(archive->conteudo.conteudo, sizeof(unsigned char), MAX_CONTEUDO, arquivo_membro);
    if (bytes_lidos == 0) {
        printf("Erro ao ler o conteúdo do membro '%s'.\n", nome_membro);
        fclose(arquivo_membro);
        return;
    }

    // Fecha o arquivo do membro
    fclose(arquivo_membro);

    printf("Membro '%s' inserido com sucesso no Archive.\n", nome_membro);
}
