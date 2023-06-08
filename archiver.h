#include <sys/stat.h>
#include <stdio.h>

#define ARQUIVO_VERIFICADO 0 // constante caso sucesso
#define ERRO_NA_ABERTURA -1
#define ARQUIVO_NAO_ENCONTRADO -2
#define ERRO_FSEEK -3
#define ERRO_FTELL -4
#define QUANTIDADE_MAXIMA 1000 // quantidade maxima de arquivos no diretorio
#define BUFFER_SIZE 1024

typedef struct { 

    char nome[100];
    int user_ID;
    int permissoes;
    long tamanho;
    time_t data_modificacao;
    int ordem;
    long localizacao;

}Membro;

typedef struct {
    Membro* membros[QUANTIDADE_MAXIMA];
    int num_membros;
} Archive;

typedef struct {

    Archive archive;

}Archiver;

void inicializar_membro(Membro** membro, const char* nome);

void inicializar_archive(Archive * archive);

void preencher_dados_membro(Membro* membro, const char* nome_arquivo);

void adicionar_membro(Archive* archive, const char* nome_membro, const char* nome_arquivo_destino);

void substituir_membro(Membro* novo_membro, int indice_existente, Membro* membro_existente);

int encontrar_nome (Archive* archive, const char* nome_membro);

int adiciona_verificando_data(Archive* archive, const char* nome_membro, const char* arquivo_destino);

void atualizar_diretorio(Archive* archive, FILE* arquivo);

void escrever_arquivo_archive(Archive* archive);

int mover_membro(Archive *archive, const char* membro_frente, const char* membro_atras);

void extrair_membros(Archive *archive, const char* nome_membro);

void listar_conteudo(const char* nome_arquivo);

void remover_membro(const char* nome_arquivo, const char* nome_membro);

void exibir_ajuda();