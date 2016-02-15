CC      = gcc
CFLAGS  = -g
RM      = rm -f
BIN	= ./bin/

default: all

all: $(BIN)hello

hello.o: hello.c
	$(CC) $(CFLAGS) -c -o hello.o hello.c

$(BIN)hello: hello.o
	$(CC) hello.o -o $(BIN)hello

clean:
	$(RM) $(BIN)hello
	$(RM) hello.o
