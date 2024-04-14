#include "archiver.h"

/* lista os membros do archive */
void listar_membros(const Archiver *archiver) {
    printf("\n-----------------------\n");
    printf("Membros no ArchiveData:");
    printf("\n-----------------------\n");

    for (int i = 0; i < archiver->diretorio.num_membros; i++) {
        const Membro *membro = &(archiver->diretorio.membros[i]);
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