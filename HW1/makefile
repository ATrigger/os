CC      = gcc
CFLAGS  = -g
RM      = rm -f


default: all

all: hello

hello.o: hello.c
	$(CC) $(CFLAGS) -c -o hello.o hello.c

hello: hello.o
	$(CC) hello.o -o ./bin/hello

clean:
	$(RM) hello
	$(RM) hello.o
