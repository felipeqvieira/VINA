#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "lista.h"
#include "vetorlista.h"
#include "funcoes_gerais.h"
#include "livrocifra.h"
#include "pega_chaves.h"
#include "decodifica_mensagem.h"
#include "codifica_mensagem.h"

// PROBLEMAS CORRIGIDOS: LE QUALQUER LIVRO CIFRA, DECIFRA QUAISQUER MENSAGENS.
// ALTERACOES NECESSARIAS: TODAS AS MUDANCAS FORAM FEITAS NO LIVROCIFRA.C

#define STRING_SIZE 1024 // constante para optarg

/* funcao main */
int main(int argc, char **argv){

    int flag_e = 0;
    int flag_d = 0; 

    char *bvalue = malloc(sizeof(STRING_SIZE));
    char *cvalue = malloc(sizeof(STRING_SIZE));
    char *ivalue = malloc(sizeof(STRING_SIZE));
    char *mvalue = malloc(sizeof(STRING_SIZE));
    char *ovalue = malloc(sizeof(STRING_SIZE));

    int option;

    opterr = 0;

    while ( (option = getopt(argc,argv,"b:c:dei:m:o:")) != -1){

        switch(option){

                case'e': //opcao a foi setada
                    flag_e = 1;
                    break;

                case'd': //opcao d foi setada
                    flag_d = 1;
                    break;

                case'b': //opcao b foi setada
                    bvalue = optarg; 
                    break;

                case'i': //opcao i foi setada
                    ivalue = optarg; 
                    break;

                case'm': //opcao m foi setada
                    mvalue = optarg; 
                    break;

                case'o': //opcao o foi setada
                    ovalue = optarg; 
                    break;

                case'c': //opcao c foi setada
                    cvalue = optarg;
                    break;
                
                default:
                    fprintf(stderr, "Uso indevido de opcao\n");
                    break;

        }
    }

    if (flag_e == 1){ 

        // Codificar uma mensagem qualquer contida em um arquivo ascii usando um livro
        //cifra
        if (bvalue != NULL && mvalue != NULL && ovalue != NULL && cvalue != NULL){

            FILE *livro_cifra;

            livro_cifra = fopen(bvalue, "r");

            if (! livro_cifra){
                printf("Erro ao abrir arquivo!\n");
                return -1;
            }

            FILE *mensagem_original;

            mensagem_original = fopen(mvalue,"r");

            if (! mensagem_original){
                printf("Erro ao abrir arquivo!\n");
                return -1;
            }

            FILE *mensagem_codificada;

            mensagem_codificada = fopen(ovalue, "w");

            if (! mensagem_codificada){
                printf("Erro ao abrir arquivo!\n");
                return -1;
            }

            FILE *arquivo_de_chaves;

            arquivo_de_chaves = fopen(cvalue, "w");

            if (! arquivo_de_chaves){
                printf("Erro ao abrir arquivo!\n");
                return -1;
            }

            /* inicializa listas */
            vetor_lista *V = inicializar_vetor_lista(); 

            /* pega as chaves do livro cifra */
            livrocifra(livro_cifra, V);
            
            /* imprime as chaves no arquivo */
            imprimir_arquivo(V, arquivo_de_chaves);
            
            /* codifica a mensagem */
            codifica(mensagem_original,V,mensagem_codificada);

            /* destruir_listas(V);*/
            
            return 1;

        }else 
            return -1;
    }
    
    if(flag_d == 1){

        char *ptr;
        char texto[4] ="txt";

        if ( (ptr = strstr(cvalue,texto)) != 0){

            if(ivalue != NULL && ovalue != NULL){
                
                FILE *mensagem_codificada = fopen(ivalue, "r");

                if (! mensagem_codificada){
                    printf("Erro ao abrir arquivo!\n");
                    return -1;
                }

                FILE *arquivo_de_chaves = fopen(cvalue, "r");

                if (! arquivo_de_chaves){
                    printf("Erro ao abrir arquivo\n");
                    return -1;
                }

                FILE *mensagem_decodificada = fopen(ovalue, "w");

                if (! mensagem_decodificada){
                    printf("Erro ao abrir arquivo\n");
                    return -1;
                }

                /* inicializa listas */
                vetor_lista *V = inicializar_vetor_lista();

                /* retira chave do arquivo de chaves */
                retira_chave(arquivo_de_chaves, V);

                /* decodifica a mensagem */
                decodifica(mensagem_codificada, V, mensagem_decodificada);

                /* destruir_listas(V); */
            }
        
        } else if ((ptr = strstr(bvalue,texto)) != 0){
            
            if(ivalue != NULL && ovalue != NULL){

                FILE *mensagem_codificada = fopen(ivalue,"r");

                if (! mensagem_codificada){
                    printf("Erro ao abrir arquivo!(mensagem codificada)\n");
                    return -1;
                }

                FILE *livro_cifra = fopen(bvalue,"r");

                if (! livro_cifra){
                    printf("Erro ao abrir arquivo!(livrocifra)\n");
                    return -1;
                }

                FILE *mensagem_decodificada = fopen(ovalue,"w");

                if (! mensagem_decodificada){
                    printf("Erro ao abrir arquivo!(mensagem decodificada)\n");
                    return -1;
                }

                vetor_lista *V = inicializar_vetor_lista();

                /* retira chaves do livro cifra */
                livrocifra(livro_cifra, V);

                /* decodifica a mensagem */
                decodifica(mensagem_codificada, V, mensagem_decodificada);

                /* destruir_listas(V); */
            }
        }
    
    }
    
    return 1;
}