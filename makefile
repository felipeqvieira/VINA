CFLAGS = -Wall -std=c99 -g
LDFLAGS = -lm
SRCDIR = src
OBJDIR = obj
SOURCES = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/**/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))
CC = gcc

all: vina++

vina++: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS)

purge: clean
	rm -f vina++
