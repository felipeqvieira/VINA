int verifica_nome(Archiver *archive, const char *nome_arquivo);

int contar_membros(char **membros);

void extrair_informacoes_membro(Archiver *archiver, const char *nome_arquivo, Membro *membro);

void verificar_archive_existente(Archiver *archiver, const char *nome_arquivo);

void criar_diretorio(const char *caminho);

void atualizar_localizacao_membros(Archiver *archiver);