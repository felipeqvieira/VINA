#include "archiver.h"
#include "func_comp.h"

void extrair_membro(Archiver *archiver, char **nomes_membros, int num_membros){

  //Abre o arquivo em modo de leitura binária
  FILE *arquivo_archive = fopen(archiver->nome_archive, "rb");

  if(! arquivo_archive){
    printf("Erro ao abrir o arquivo do archive '%s'.\n", archiver->nome_archive);
    return;
  }

  //obtém o tamanho do arquivo sem considerar o diretório
  fseek(arquivo_archive, 0, SEEK_END);
  long tamanho_arquivo = ftell(arquivo_archive);

  fseek(arquivo_archive, 0, SEEK_SET);
  tamanho_arquivo -= sizeof(Diretorio);

  //verifica se todos os membros devem ser extraídos
  int extrair_todos = (num_membros == 0);

  //Percorre a lista de membros e extrai cada um
  for(int i = 0; i < archiver->diretorio.num_membros; i++){

    Membro *membro = &(archiver->diretorio.membros[i]);
    const char *nome_membro = membro->nome;

    int extrair_membro = extrair_todos;

    for(int j = 0; j < num_membros; j++){
      if(strcmp(nome_membro, nomes_membros[j]) == 0){
        extrair_membro = 1;
        break;
      }
    }

    if (!extrair_membro){
      continue;
    }

    char *diretorio_membro = strdup(nome_membro);
    char *ultimo_slash = strrchr(diretorio_membro, '/');

    if(ultimo_slash){
      *ultimo_slash = '\0';
      criar_diretorio(diretorio_membro);
    }

    free(diretorio_membro);

    long deslocamento = 0;

    for(int j = 0; j < i; j++){
      deslocamento += archiver->diretorio.membros[j].tamanho;
    }

    fseek(arquivo_archive, deslocamento, SEEK_SET);

    FILE *arquivo_membro = fopen(nome_membro, "wb");

    if(! arquivo_membro){
      printf("Erro ao criar o arquivo do membro '%s'.\n", nome_membro);
      continue;
    }

    long tamanho_membro = membro->tamanho;

    unsigned char buffer[MAX_CONTEUDO];

    int quociente = tamanho_membro/MAX_CONTEUDO;

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

    int resto = tamanho_membro % MAX_CONTEUDO;

    if (resto > 0){
      size_t bytes_lidos = fread(buffer, sizeof(unsigned char), resto, arquivo_archive);

      if (bytes_lidos == 0){
        printf("Erro ao ler o conteúdo do arquivo.\n");
        fclose(arquivo_membro);
        remove(nome_membro);
        break;
      }

      size_t bytes_gravados = fwrite(buffer, sizeof(unsigned char), bytes_lidos, arquivo_membro);

      if (bytes_gravados < bytes_lidos){
        printf("Erro ao gravar o conteúdo no arquivo extraído.\n");
        fclose(arquivo_membro);
        remove(nome_membro);
        break;
      }
    }

    fclose(arquivo_membro);
    
  } 

  fclose(arquivo_archive);
}