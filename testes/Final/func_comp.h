void listar_membros(const ArchiveData *archiveData);

int arquivo_existe(const char *nome_arquivo);

void verificar_conteudo_membro(const char *nome_arquivo, const ConteudoMembro *conteudo_membro, size_t tamanho_arquivo);

ArchiveData* inicializar_archive(const char *nome_archive);

int verifica_nome(const ArchiveData *archiveData, const char *nome_arquivo);

void extrair_informacoes_membro(const char *nome_arquivo, Membro *membro);

int contar_membros(char **membros);

int verificar_tempo_modificacao_mais_recente(const char* nome_membro, Membro* membro_existente);

void substituir_recente(Archiver* archiver, int posicao_existente, Membro* novo_membro, unsigned char* conteudo, long tamanho_conteudo);