CFLAGS = -Wall -std=c99 -g
LDFLAGS = -lm
OBJS = vina++.o ajuda.o archiver.o extrair.o func_comp_archive.o func_comp_membro.o inserir.o listar.o remover.o
CC = gcc

all: vina++

vina++: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

vina++.o: vina++.c
	$(CC) -c $(CFLAGS) $<

ajuda.o: ajuda.c
	$(CC) -c $(CFLAGS) $<

archiver.o: archiver.c
	$(CC) -c $(CFLAGS) $<

extrair.o: extrair.c
	$(CC) -c $(CFLAGS) $<

func_comp_archive.o: func_comp_archive.c
	$(CC) -c $(CFLAGS) $<

func_comp_membro.o: func_comp_membro.c
	$(CC) -c $(CFLAGS) $<

inserir.o: inserir.c
	$(CC) -c $(CFLAGS) $<

remover.o: remover.c
	$(CC) -c $(CFLAGS) $<

listar.o: listar.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJS)

purge: clean
	rm -f vina++
