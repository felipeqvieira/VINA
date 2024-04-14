#include "archiver.h"
#include "func_comp.h"

void remover_membros(Archiver *archiver, char **nomes_membros, int num_membros){

  FILE *arquivo_original = fopen(archiver->nome_archive, "rb");

  if(! arquivo_original){
    printf("Erro ao abrir o arquivo original do archive.\n");
    return;
  }

  FILE *arquivo_temporario = fopen("temp_archive", "wb");

  if(! arquivo_temporario){
    printf("Erro ao criar o arquivo temporário do archive.\n");
    fclose(arquivo_original);
    return;
  }

  int num_membros_atualizado = archiver->diretorio.num_membros;

  int num_membros_removidos = 0;

  for(int i = 0; i < archiver->diretorio.num_membros; i++){

    Membro *membro = &(archiver->diretorio.membros[i]);

    int membro_removido = 0;

    for (int j = 0; j < num_membros; j++){
      if(strcmp(membro->nome, nomes_membros[j]) == 0){
        membro_removido = 1;
        num_membros_atualizado--;
        num_membros_removidos++;
        break;
      }
    }

    if(! membro_removido){
      if(num_membros_removidos > 0){
        archiver->diretorio.membros[i - num_membros_removidos] = *membro;
      }
    }

  }

  archiver->diretorio.num_membros = num_membros_atualizado;

  for(int i = 0; i < num_membros_atualizado; i++){

    Membro *membro = &(archiver->diretorio.membros[i]);

    long deslocamento = membro->localizacao;

    fseek(arquivo_original, deslocamento, SEEK_SET);

    long tamanho_membro = membro->tamanho;

    unsigned char buffer[MAX_CONTEUDO];

    int quociente = tamanho_membro / MAX_CONTEUDO;

    for(int j = 0; j < quociente; j++){
      fread(buffer, sizeof(unsigned char), MAX_CONTEUDO, arquivo_original);
      fwrite(buffer, sizeof(unsigned char), MAX_CONTEUDO, arquivo_temporario);
    }

    int resto = tamanho_membro % MAX_CONTEUDO;

    if(resto > 0){
      fread(buffer, sizeof(unsigned char), resto, arquivo_original);
      fwrite(buffer, sizeof(unsigned char), resto, arquivo_temporario);
    }

  }

  fseek(arquivo_temporario, 0, SEEK_END);

  fwrite(&(archiver->diretorio), sizeof(Diretorio), 1, arquivo_temporario);

  fclose(arquivo_original);
  fclose(arquivo_temporario);

  remove(archiver->nome_archive);

  rename("temp_archive", archiver->nome_archive);

  atualizar_localizacao_membros(archiver);

}