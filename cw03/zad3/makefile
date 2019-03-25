all: build clean

build:
	gcc -Wall -c monitor.c
	gcc -Wall -c tester.c
	gcc -Wall monitor.o -o monitor
	gcc -Wall tester.o -o tester
clean:
	rm -f *.o

