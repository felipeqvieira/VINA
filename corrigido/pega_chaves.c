/* Incluindo Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include "lista.h"
#include <string.h>
#include "vetorlista.h"
#include "funcoes_gerais.h"
#include "pega_chaves.h"

/* Constante para vetor lista */
#define N_CARACTERES 35

/* pega primeiro caractere da linha */
int primeiro_caractere_p(char *linha, vetor_lista *V){

    /* pega o primeiro caractere da linha */
    int posicao = linha[0];

   if (posicao >= 48 && posicao <= 57){
        return posicao-22;
   } else if (posicao >= 97 && posicao <= 122){
            return posicao-97;
        }

    return 0;

}

void pega_chave(char *linha, vetor_lista *V, int letra){

    /* cria ponteiro para string busca*/
    char *busca;

    /* inicializa chave */
    int chave = 0;

    /* soma 2 com linha para evitar letra e : */
    linha = linha + 2;
    
    /* o delimitador do strtok vai ser o espaco */
    char delim[] = " ";

    /* acha a primeira ocorrencia de espaco */
    busca = strtok(linha,delim);

    /* while para continuar achando as ocorrencias */
    while (busca != NULL){

        /* pega a chave */
        sscanf(busca,"%d",&chave);

        /* insere na lista */
        adiciona_ordem_lista(V[letra].l,chave);

        /* procura outra ocorrencia*/
        busca = strtok(NULL,delim);
    }
}


/* funcao principal para pegar as chaves*/
void retira_chave(FILE *arq, vetor_lista *V){

    /* calcula numero de caracteres do arquivo */
    int caracteres = conta_caracteres(arq);

    /* aloca linha */
    char *linha = malloc(sizeof(char)*(caracteres+1));   

    /* inicializa variavel letra */
    int letra = 0;

    /* reseta ponteiro do arquivo */
    rewind(arq);

    /* while para pegar linhas do arquivo */
    while (fgets(linha,caracteres,arq) != NULL){
       
        /* pega primeiro caractere */
        letra = primeiro_caractere_p(linha, V);
        
        /* pega as chaves */
        pega_chave(linha, V, letra);

    }

    free(linha);

}

