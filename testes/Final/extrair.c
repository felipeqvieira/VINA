#include "archiver.h"
#include "func_comp.h"

void extrair_membros(Archiver* archiver, char** membros, int num_membros) {
    // Verificar se o arquivo "archive" existe e pode ser aberto
    FILE* arquivo = fopen(archiver->archiveData.nome_archive, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de archive.\n");
        return;
    }
    
    // Criar diretório para a extração dos membros
    char diretorio_extraido[MAX_NOME_ARCHIVE];
    snprintf(diretorio_extraido, MAX_NOME_ARCHIVE, "%s_extraido", archiver->archiveData.nome_archive);
    if (mkdir(diretorio_extraido) != 0) {
        printf("Erro ao criar o diretório de extração.\n");
        fclose(arquivo);
        return;
    }
    
    // Verificar se todos os membros devem ser extraídos
    int extrair_todos = (num_membros == 0);
    
    // Ler os membros indicados para extração
    for (int i = 0; i < archiver->archiveData.diretorio.num_membros; i++) {
        Membro* membro = &(archiver->archiveData.diretorio.membros[i]);
        
        // Verificar se o membro deve ser extraído
        int extrair = extrair_todos;
        if (!extrair && num_membros > 0) {
            for (int j = 0; j < num_membros; j++) {
                if (strcmp(membros[j], membro->nome) == 0) {
                    extrair = 1;
                    break;
                }
            }
        }
        
        if (extrair) {
            // Criar o caminho completo para o arquivo extraído
            char caminho_arquivo_extraido[MAX_NOME_ARCHIVE + MAX_NOME];
            snprintf(caminho_arquivo_extraido, MAX_NOME_ARCHIVE + MAX_NOME, "%s/%s", diretorio_extraido, membro->nome);
            
            // Criar o arquivo extraído
            FILE* arquivo_extraido = fopen(caminho_arquivo_extraido, "wb");
            if (arquivo_extraido == NULL) {
                printf("Erro ao criar o arquivo extraído: %s\n", membro->nome);
                continue;
            }
            
            // Escrever o conteúdo do membro no arquivo extraído
            fwrite(archiver->archiveData.conteudo.conteudo + membro->localizacao, sizeof(unsigned char), membro->tamanho, arquivo_extraido);
            
            // Fechar o arquivo extraído
            fclose(arquivo_extraido);
            
            printf("Membro extraído: %s\n", membro->nome);
        }
    }
    
    // Fechar o arquivo de archive
    fclose(arquivo);
}
