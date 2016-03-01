CC      = gcc
CFLAGS  = -g
RM      = rm -f
BIN	= ./

default: all

all: $(BIN)hello_world

hello.o: hello.c
	$(CC) $(CFLAGS) -c -o hello.o hello.c

$(BIN)hello_world: hello.o
	$(CC) hello.o -o $(BIN)hello_world

clean:
	$(RM) $(BIN)hello_world
	$(RM) hello.o
