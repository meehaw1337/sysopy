cmd = gcc -Wall -ggdb

all: static shared dynamic

opt_zero:
	make all optlvl=0

opt_one:
	make all optlvl=1

opt_s:
	make all optlvl=s

static:
	$(cmd) -c library.c -O$(optlvl)
	ar rcs library.a library.o
	$(cmd) -c main.c -O$(optlvl)
	$(cmd) main.o library.a -o main -O$(optlvl)
	rm -f *.o
	make tests

shared:
	$(cmd) -c -fPIC library.c -O$(optlvl)
	$(cmd) -shared -fPIC -o library.so library.o -O$(optlvl)
	$(cmd) main.c -o main -L. library.so -Wl,-rpath=`pwd` -O$(optlvl)
	rm -f *.o
	make tests

dynamic:
	$(cmd) -c -fPIC library.c -O$(optlvl)
	$(cmd) -shared -fPIC -o library.so library.o -O$(optlvl)
	$(cmd) main.c -o main -L. -ldl -D DYNAMIC -O$(optlvl)
	rm -f *.o
	make tests	

clean:
	rm -f *.o


tests:
	./main search_directory bardzoduzy plikjeden tmp1
	./main search_directory duzy plikjeden tmp1
	./main search_directory sredni plikjeden tmp1
	./main create_table 1 search_directory bardzoduzy plikjeden tmp1
	./main create_table 1 search_directory duzy plikjeden tmp1
	./main create_table 1 search_directory sredni plikjeden tmp1
	./main create_table 1 search_directory bardzoduzy plikjeden tmp1 remove_block 0
	./main create_table 1 search_directory duzy plikjeden tmp1 remove_block 0
	./main create_table 1 search_directory sredni plikjeden tmp1 remove_block 0
	./main create_table 3 search_directory bardzoduzy plikjeden tmp1 remove_block 0 search_directory duzy plikjeden tmp2 remove_block 0 search_directory sredni plikjeden tmp3 remove_block 0
	
