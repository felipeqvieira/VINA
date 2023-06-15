#include <stdio.h>

void adicionar_membro(const char* nome_membro, const char* nome_arquivo_destino) {
    FILE* arquivo_destino = fopen(nome_arquivo_destino, "ab");
    if (arquivo_destino == NULL) {
        printf("Erro ao abrir arquivo destino.\n");
        return;
    }

    FILE* membro = fopen(nome_membro, "rb");
    if (membro == NULL) {
        printf("Erro ao abrir arquivo membro.\n");
        fclose(arquivo_destino);
        return;
    }

    char buffer[1024];
    size_t bytes_lidos;

    while ((bytes_lidos = fread(buffer, 1, sizeof(buffer), membro)) > 0) {
        fwrite(buffer, 1, bytes_lidos, arquivo_destino);
    }

    fclose(membro);
    fclose(arquivo_destino);

    printf("Membro adicionado com sucesso!\n");
}

int main() {
    const char* nome_membro = "lorem.txt";
    const char* nome_archive = "felipe.vpp";

    adicionar_membro(nome_membro, nome_archive);

    return 0;
}