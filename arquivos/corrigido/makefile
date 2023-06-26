CFLAGS = -Wall -std=c99 -g
LDFLAGS = -lm
OBJS = beale.o livrocifra.o funcoes_gerais.o  decodifica_mensagem.o codifica_mensagem.o pega_chaves.o vetorlista.o lista.o
CC = gcc

all: beale

beale: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

beale.o: beale.c
	$(CC) -c $(CFLAGS) $<

funcoes_gerais.o: funcoes_gerais.c
	$(CC) -c $(CFLAGS) $<

decodifica_mensagem.o: decodifica_mensagem.c
	$(CC) -c $(CFLAGS) $<

codifica_mensagem.o: codifica_mensagem.c
	$(CC) -c $(CFLAGS) $<

livrocifra.o: livrocifra.c
	$(CC) -c $(CFLAGS) $<

lista.o: lista.c
	$(CC) -c $(CFLAGS) $<

vetorlista.o: vetorlista.c
	$(CC) -c $(CFLAGS) $<

pega_chaves.o: pega_chaves.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJS)

purge: clean
	rm -f beale
