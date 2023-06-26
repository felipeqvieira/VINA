/* Incluindo Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include "lista.h"
#include <string.h>
#include "vetorlista.h"

/* Constante para numero de caracteres do vetor */
#define N_CARACTERES 35

/* funcao que incializa vetor lista */
vetor_lista * inicializar_vetor_lista(){

    /* aloca vetor lista */
    vetor_lista *V = malloc(sizeof(vetor_lista)*N_CARACTERES);

    /* inicializa lista para cada um dos indices das letras do alfabeto */
    for (int i = 0; i <= N_CARACTERES; i++){

        V[i].l = cria_lista();

    }

    return V;
}

/* funcao que destroi vetor lista */
vetor_lista * destruir_listas (vetor_lista *V){

    /* destroi lista de todos os indices */
    for (int i = 0; i <= N_CARACTERES; i++)
        destroi_lista(V[i].l);

    /* da free no vetor lista */
    free(V);

    return NULL;

}
