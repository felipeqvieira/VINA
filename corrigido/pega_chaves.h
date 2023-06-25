/* pega primeiro caractere da linha */
int primeiro_caractere_p(char *linha, vetor_lista *V);

/* pega as chaves restantes */
void pega_chave(char *linha, vetor_lista *V, int letra);

/* funcao principal para pegar as chaves */
void retira_chave(FILE *arq, vetor_lista *V);