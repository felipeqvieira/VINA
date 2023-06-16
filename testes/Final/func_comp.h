void listar_membros(const ArchiveData *archiveData);

int arquivo_existe(const char *nome_arquivo);

void verificar_conteudo_membro(const char *nome_arquivo, const ConteudoMembro *conteudo_membro, size_t tamanho_arquivo);

ArchiveData* inicializar_archive(const char *nome_archive);

int verifica_nome(const ArchiveData *archiveData, const char *nome_arquivo);

void extrair_informacoes_membro(const char *nome_arquivo, Membro *membro);

int contar_membros(char **membros);