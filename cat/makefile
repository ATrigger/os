CC      = gcc
CFLAGS  = -g
RM      = rm -f
BIN	= ./

default: all

all: $(BIN)cat_world

cat.o: cat.c
	$(CC) $(CFLAGS) -c -o cat.o cat.c

$(BIN)cat_world: cat.o
	$(CC) cat.o -o $(BIN)cat

clean:
	$(RM) $(BIN)cat
	$(RM) cat.o
