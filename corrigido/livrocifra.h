/* transforma uma letra maiuscula em minuscula*/
int transforma_minuscula(char letra);

/* funcao que pega o primeiro caractere da linha */
int primeiro_caractere_l(char *linha, vetor_lista *V, int posicao);

/* funcao que procura as primeiras letras pela ocorrencia de espacos */
int pega_linha(char *linha, vetor_lista * V, int comeco, int posicao);

/* funcao responsavel pelas linhas da string */
void livrocifra (FILE *arq, vetor_lista *V);
