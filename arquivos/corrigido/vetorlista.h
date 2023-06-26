/*          VETOR LISTA         */

/* struct vetor lista */
typedef struct vetor_lista{

    lista_t *l;

}vetor_lista;

/* Inicializa o vetor lista*/
vetor_lista * inicializar_vetor_lista();

/* destroi vetor lista */
vetor_lista * destruir_listas (vetor_lista *V);
