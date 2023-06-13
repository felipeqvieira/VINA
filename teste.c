#include "archiver.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#define STRING_SIZE 1024

/* funcao main */
int main(int argc, char **argv){

    int flag_e = 0;
    int flag_d = 0; 

    char *value = malloc(sizeof(STRING_SIZE));

    int option;

    opterr = 0;

    while ( (option = getopt(argc,argv,"b:c:dei:m:o:")) != -1){

        switch(option){

                case'i': //opcao a foi setada
                    flag_e = 1;
                    break;

                case'a': //opcao d foi setada
                    flag_d = 1;
                    break;

                case'm': //opcao b foi setada
                    value = optarg; 
                    break;

                case'x': //opcao i foi setada
                    value = optarg; 
                    break;

                case'r': //opcao m foi setada
                    value = optarg; 
                    break;

                case'c': //opcao o foi setada
                    value = optarg;
                    listar_conteudo(value);
                    break;

                case'h': //opcao c foi setada
                    exibir_ajuda();
                    break;
                
                default:
                    fprintf(stderr, "Uso indevido de opcao\n");
                    break;

        }
    }
}