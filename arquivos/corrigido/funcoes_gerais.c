#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "lista.h"
#include "vetorlista.h"
#include "funcoes_gerais.h"

#define N_CARACTERES 35

/*          Funcoes gerais      */

/* conta caracteres do arquivo */
int conta_caracteres (FILE *arq){
    
    char c;

    int count = 0;

    for (c = getc(arq); c!=EOF; c= getc(arq))
        count++;

    return count;
}


/* pega um numero aleatorio da lista */
int pega_aleatorio(lista_t *l){

    /* verifica tamanho da lista */
    int tamanho = tamanho_lista(l);

    /* inicia aleatoriedade*/
    srand(time(0));

    /* se nao tiver nada na lista, retorna -1*/
    if (tamanho == 0)
        return -1;
            /* se tiver soh um elemento, retorna o primeiro*/
    else if (tamanho == 1)
                return l->Primeiro->dado;
            else{
                /* cria ponteira auxiliar*/
                nodo_lista_t *aux;
                /* aponta para o primeiro*/
                aux = l->Primeiro;

                /* pega numero aleatorio de 0 ate tamanho */
                int r = rand() % tamanho+1;

                /* se o numero aleatorio for 0 ou 1, retorna o primeiro*/
                if (r <= 1)
                    return aux->dado;
                else{
                    /* senao, conta ate chegar no elemento*/
                    for (int i = 0; i < r-1; i++)
                        aux = aux->prox;
                    /* retorna valor do elemento */
                    return aux->dado;
                }
            }


}

void imprimir_arquivo(vetor_lista *V, FILE *destino){

    nodo_lista_t * aux = NULL;

    for (int i = 0; i <= N_CARACTERES; i++){
        
        /*printf("%d\n", i);*/

        if(V[i].l->Primeiro != NULL){
            /*printf("\noi\n");*/
            aux = V[i].l->Primeiro;
            
            if (i >= 0 && i <= 25)
                fprintf(destino, "\n%c: ", i + 97);
            else if (i >= 26 && i <= 35)
                fprintf(destino,"\n%c: ", i+22);
        }

        /*printf("oi2\n");*/

        while (aux != NULL){
            
            fprintf(destino,"%d ", aux->dado);

            aux = aux ->prox;
        }

    }
}
