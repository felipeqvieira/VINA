#include "archiver.h"
#include "func_comp.h"

void mover_membro(Archiver *archiver, char **nomes_membros, int num_membros){

  //move o membro indicado na linha de comando para imediatamente depois do membro target existente em archive. A movimentação deve ocorrer na seção de dados do archive

  char *target = nomes_membros[1];
  char *membro = nomes_membros[0];

  printf("Target: %s\n", target);
  printf("Membro: %s\n", membro);

  int target_index = -1;

  for(int i = 0; i < archiver->diretorio.num_membros; i++){
    if(strcmp(archiver->diretorio.membros[i].nome, target) == 0){
      printf("Target encontrado\n");
      target_index = i;
      break;
    }
  }

  for(int i = 0; i < archiver->diretorio.num_membros; i++){

    printf("Membro: %s\n", archiver->diretorio.membros[i].nome);

    if(strcmp(archiver->diretorio.membros[i].nome, membro) == 0){

      printf("Membro encontrado\n");
      Membro temp = archiver->diretorio.membros[i];

      printf("i: %d\n", i);
      printf("target_index: %d\n", target_index);

      if(i < target_index){
        printf("caso1\n");
        for(int j = i; j < target_index; j++){
          archiver->diretorio.membros[j] = archiver->diretorio.membros[j+1];
        }
        archiver->diretorio.membros[target_index] = temp;
      }
      else if(i > target_index){
        printf("caso2\n");
        for(int j = i; j > target_index; j--){
          archiver->diretorio.membros[j] = archiver->diretorio.membros[j-1];
        }
        archiver->diretorio.membros[target_index] = temp;
      }

      break;
      
   } else 
    printf("Membro não encontrado\n");
  
  }

}