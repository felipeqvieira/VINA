/*          LISTA           */

/* struct lista*/
typedef struct nodo_lista{

    struct nodo_lista *prox;
    int dado;

}nodo_lista_t;

typedef struct lista{

    nodo_lista_t *Primeiro;
    int tamanho;

}lista_t;


/* Cria uma lista vazia */
lista_t * cria_lista ();

/* Destroi a lista, sempre retorna NULL */
lista_t * destroi_lista (lista_t *l);

/*Adiciona na lista em ordem */
int adiciona_ordem_lista (lista_t *l, int elemento);

/* verifica se ha um inteiro na lista */
int busca_inteiro (lista_t *l, int c);

/* Informa o tamanho da lista */
int tamanho_lista (lista_t *l);