cmd = gcc -Wall -ggdb

all: static shared clean

static:
	$(cmd) -c library.c
	ar rcs library.a library.o

shared:
	$(cmd) -c -fPIC library.c
	$(cmd) -shared -fPIC -o library.so library.o

clean:
	rm -f *.o
