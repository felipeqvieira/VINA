/* Incluindo Bibliotecas */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lista.h"
#include "vetorlista.h"
#include "codifica_mensagem.h"
#include "funcoes_gerais.h"

#define NUMERO_STRING 35

int codifica(FILE *arq, vetor_lista *V, FILE *destino){

    /* conta caracteres do arquivo */
    int caracteres = conta_caracteres(arq);
    int numero = 0;
    int espaco = -1;
    int i = 0;
    int pula_linha = 10;

    /* aloca linha que contem tamanho suficiente para o arquivo */
    char *linha = malloc(sizeof(char)*(caracteres+1));

    /* reseta o ponteiro do arquivo */
    rewind(arq);

    /* while para pegar linha do arquivo */
    while (fgets(linha,caracteres,arq) != NULL){

        for (i = 0; i < strlen(linha); i++){

            if ( (linha[i] == 32))
                fprintf(destino, "%d ", espaco);
            else if (linha[i] == 10)
                fprintf(destino,"%c",pula_linha);
                else if (linha[i] >= 48 && linha[i] <= 57){
                    numero = pega_aleatorio(V[linha[i]-22].l);
                    fprintf(destino, "%d ", numero);
                    }else if (linha[i] >= 97 && linha[i] <= 122){
                            numero = pega_aleatorio(V[linha[i]-97].l);
                            fprintf(destino, "%d ", numero);
                        }

        }
    }

    free(linha);
       
    return 1;

}
