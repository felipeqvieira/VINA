/* Incluindo Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"
#include "vetorlista.h"
#include "funcoes_gerais.h"
#include "livrocifra.h"

/* Constante para numero de caracteres do vetor */
#define N_CARACTERES 35

/*          LIVRO CIFRA             */

/* funcao que muda para minuscula uma letra maiuscula */
int transforma_minuscula(char letra){

    letra = letra + 32;

    return letra;
}

/* funcao que pega o primeiro caractere da linha */
int primeiro_caractere_l(char *linha, vetor_lista *V, int posicao){

    int i = 0; int comeco = 0; int letra = 0; int tamanholinha = strlen(linha);

    while (i < 1){
        /* tudo que nao for letra e numero */
        if ( (linha[comeco] < 48 || linha[comeco] > 122) || (linha[comeco] >= 58 && linha[comeco] <= 64)  || (linha[comeco] >= 91 && linha[comeco] <= 96)){
            comeco = comeco + 1;

            /*printf("comeco = %d\n", comeco);
            printf("tamanho da linha = %d\n", tamanholinha);
            */

            if (comeco > tamanholinha){
                return -1;
            }
            
        }
        /* recebe letras maiusculas */
        else if ( (linha[comeco] >= 65) && (linha[comeco] <= 90) ){
            letra = transforma_minuscula(linha[comeco]);
            adiciona_ordem_lista(V[letra-97].l,posicao);
            i = 1;
        }
        /* recebe letras minusculas e numeros */
        else if ( (linha[comeco] >= 48 && linha[comeco] <= 57) || (linha[comeco] >= 97 && linha[comeco] <= 122) ){

            if (linha[comeco] >= 48 && linha[comeco] <= 57){
                letra = linha[comeco];
                adiciona_ordem_lista(V[letra-22].l,posicao);
                i = 1;
            } else{
                letra = linha[comeco];
                adiciona_ordem_lista(V[letra-97].l, posicao);
                i = 1;
            }
        }

    }

    return comeco;  
}


/* Funcao que procura as primeiras letras pela ocorrencia de espacos */
int pega_linha(char *linha, vetor_lista * V, int comeco, int posicao){

    /* busca aponta para linha */
    char *busca = linha;
    /* soma comeco com busca para evitar a primeira palavra */
    busca = busca + comeco;
    /* aloca encontros */
    char *encontros = malloc(sizeof(char)*strlen(linha));
    /* inicializa variavel letra */
    int letra = 0;
    int j = 0;
    int verifica = 0;
    
    /*printf("\nbusca1:");
    puts(busca);
    */

    /* while para encontrar cada palavra e separar elas */
    while ((busca = strstr(busca," ")) != NULL){

        /*printf("\nbusca2:");
        puts(busca);
        */

        /* soma para evitar espaco */
        busca = busca + 1;

        /* transfere o que ta em busca para encontros */
        /*memcpy(encontros, busca, sizeof(char)*strlen(busca)); */
        strcpy(encontros,busca);

        /*printf("\nencontros:");
        puts(encontros);
        */

        while (j < strlen(linha) && verifica == 0){

            if (encontros[j] == 32){
                verifica = 1;
            } else {

                /*printf("j = %d\n", j);*/
       
                /* se for letra maiuscula */
                if (encontros[j] >= 65 && encontros[j] <= 90){
                    posicao = posicao + 1;
                    letra = transforma_minuscula(encontros[j]);
                    /*printf("letra maiuscula = %c\n", letra);*/
                    adiciona_ordem_lista(V[letra-97].l, posicao);
                    verifica = 1;
                }
        
                /* caso nao seja letra maiuscula*/
        
                else if ( (encontros[j] >= 48 && encontros[j] <= 57) || (encontros[j] >= 97 && encontros[j] <= 122) ){ 
                    letra = encontros[j];
                    if (letra >= 48 && letra <= 57){
                        posicao = posicao + 1;
                        /*printf("numero = %c\n", letra);*/
                        adiciona_ordem_lista(V[letra-22].l,posicao);
                        verifica = 1;
                    }
                    else if (letra >= 97 && letra <= 122){
                        posicao = posicao + 1;
                        /*printf("letra = %c\n", letra);*/
                        adiciona_ordem_lista(V[letra-97].l, posicao);
                        verifica = 1;
                    }
                }

                if ( (encontros[j] < 48) || (encontros[j] > 57 && encontros[j] < 65) || (encontros[j] > 90 && encontros[j] < 97) || (encontros[j] > 122) )
                    j = j + 1;
            }

        }

        j = 0;
        verifica = 0;
    }

    free(encontros);
    return posicao+1;
}


/* Funcao responsavel pelas linhas da string */
void livrocifra (FILE *arq, vetor_lista *V){

    /* inicializa variaveis posicao e comeco */
    int posicao = 0;
    int comeco = 0;

    /* conta caracteres do arquivo */
    int caracteres = conta_caracteres(arq);

    /* aloca linha que contem tamanho suficiente para o arquivo */
    char *linha = malloc(sizeof(char)*(caracteres+1));

    /* reseta o ponteiro do arquivo */
    rewind(arq);

    /* while para pegar linha do arquivo */
    while (fgets(linha,caracteres,arq) != NULL){

        /* indica em que posicao termina a primeira palavra */
        comeco = primeiro_caractere_l(linha, V, posicao); 

        /* se for diferente de -1 (linha em branco) pega as outras palavras */
        if(comeco != -1)
            posicao = pega_linha(linha, V, comeco, posicao);
            

    }
    /* free em linha */
    free(linha);
}