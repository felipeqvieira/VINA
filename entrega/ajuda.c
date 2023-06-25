void exibir_ajuda() {
    printf("Opções disponíveis:\n");
    printf("-i : insere/acrescenta um ou mais membros ao archive. Caso o membro já exista no archive, ele deve ser substituído. Novos membros são inseridos respeitando a ordem da linha de comando, ao final do archive.\n");
    printf("-x : extrai os membros indicados de archive. Se os membros não forem indicados, todos devem ser extraídos. A extração consiste em ler o membro de archive e criar um arquivo correspondente, com conteúdo idêntico, em disco.\n");
    printf("-r : remove os membros indicados de archive.\n");
    printf("-c : lista o conteúdo de archive em ordem, incluindo as propriedades de cada membro (nome, UID, permissões, tamanho e data de modificação) e sua ordem no arquivo. A saída esperada é igual ao do comando tar com as opções tvf.\n");
}