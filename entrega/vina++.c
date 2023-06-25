#include "archiver.h"
#include "func_comp.h"
#include "extrair.h"
#include "inserir.h"
#include "ajuda.h"
#include "remover.h"
#include "listar.h"

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

    

    return 0;
}