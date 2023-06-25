/* Incluindo Bibliotecas */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lista.h"
#include "vetorlista.h"
#include "funcoes_gerais.h"
#include "decodifica_mensagem.h"

#define N_CARACTERES 35

void verifica_dado (vetor_lista *V, int value, FILE *destino){

    int espaco = 32; int i = 0; int verifica = 0; 

    if (value == -1)
        fprintf(destino, "%c", espaco);
    else{
        while (verifica == 0 && i <= N_CARACTERES){

            verifica = busca_inteiro(V[i].l, value);

            if (verifica == 0)
                i++;
        }
                
        if (verifica == 1){
                    
            if (i >= 0 && i<= 25)
                fprintf(destino, "%c", i+97);
            else if (i >= 26 && i <= 35)
                fprintf(destino, "%c", i+22);        
        }

    }

}

int decodifica(FILE *arq, vetor_lista *V, FILE *destino){

    /* conta caracteres do arquivo */
    int caracteres = conta_caracteres(arq);
    int value = 0;
    char delim[] = " "; char *busca; int pula_linha = 10;

    /* aloca linha que contem tamanho suficiente para o arquivo */
    char *linha = malloc(sizeof(char)*(caracteres+1));

    /* reseta o ponteiro do arquivo */
    rewind(arq);

    /* while para pegar linha do arquivo */
    while (fgets(linha,caracteres,arq) != NULL){

        busca = strtok(linha,delim);

        while (busca != NULL){

            if((busca[0] >= 48 && busca[0] <= 57) || busca[0] == 45){
            
            sscanf(busca, "%d", &value);

            verifica_dado(V, value, destino);

            }

            busca = strtok(NULL,delim);
            
        }  
        
        fprintf(destino, "%c", pula_linha);

    }
       
    free(linha);
    
    return 1;
}