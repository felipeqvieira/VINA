#include "archiver.h"
#include "remover.h"
#include "inserir.h"
#include "func_comp.h"

//-a : mesmo comportamento da opção -i, mas a substituição de um membro existente ocorre APENAS caso o parâmetro seja mais recente que o arquivado.

void substituir_membro(Archiver *archiver, char **nomes_arquivos, int num_arquivos){

    // Insere os novos membros no início do arquivo
    for (int i = 0; i < num_arquivos; i++) {

        char *nome_membro = nomes_arquivos[i];

        // Verifica se o membro já existe no ArchiveData e só substitui se a data de modificação for mais recente

        for (int j = 0; j < archiver->diretorio.num_membros; j++) {
            Membro *membro = &(archiver->diretorio.membros[j]);
            if (strcmp(membro->nome, nome_membro) == 0) {
                struct stat arquivo_stat;
                stat(nome_membro, &arquivo_stat);

                printf("Data de modificação do arquivo: %ld\n", arquivo_stat.st_mtime);

                printf("Data de modificação do membro: %ld\n", membro->data_modificacao);

                if (arquivo_stat.st_mtime > membro->data_modificacao)
                    remover_membros(archiver, &nome_membro, 1);
                
                inserir_membros(archiver, &nome_membro, 1);
                
            }
        }
    }

}