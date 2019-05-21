CC = gcc -Wall
all: threads

threads:
	$(CC) threads.c -g -o threads -lpthread -lm -lrt

clean:
	rm -rf *.o threads

test:
	./threads 1 block coins.pgm filterjeden.txt result.pgm
	./threads 2 block coins.pgm filterjeden.txt result.pgm
	./threads 4 block coins.pgm filterjeden.txt result.pgm
	./threads 8 block coins.pgm filterjeden.txt result.pgm
	./threads 1 block obrazek.pgm filterjeden.txt result.pgm
	./threads 2 block obrazek.pgm filterjeden.txt result.pgm
	./threads 4 block obrazek.pgm filterjeden.txt result.pgm
	./threads 8 block obrazek.pgm filterjeden.txt result.pgm
	./threads 1 block coins.pgm filterdwa.txt result.pgm
	./threads 2 block coins.pgm filterdwa.txt result.pgm
	./threads 4 block coins.pgm filterdwa.txt result.pgm
	./threads 8 block coins.pgm filterdwa.txt result.pgm
	./threads 1 block obrazek.pgm filterdwa.txt result.pgm
	./threads 2 block obrazek.pgm filterdwa.txt result.pgm
	./threads 4 block obrazek.pgm filterdwa.txt result.pgm
	./threads 8 block obrazek.pgm filterdwa.txt result.pgm
	./threads 1 block coins.pgm filtertrzy.txt result.pgm
	./threads 2 block coins.pgm filtertrzy.txt result.pgm
	./threads 4 block coins.pgm filtertrzy.txt result.pgm
	./threads 8 block coins.pgm filtertrzy.txt result.pgm
	./threads 1 block obrazek.pgm filtertrzy.txt result.pgm
	./threads 2 block obrazek.pgm filtertrzy.txt result.pgm
	./threads 4 block obrazek.pgm filtertrzy.txt result.pgm
	./threads 8 block obrazek.pgm filtertrzy.txt result.pgm
	./threads 1 interleaved coins.pgm filterjeden.txt result.pgm
	./threads 2 interleaved coins.pgm filterjeden.txt result.pgm
	./threads 4 interleaved coins.pgm filterjeden.txt result.pgm
	./threads 8 interleaved coins.pgm filterjeden.txt result.pgm
	./threads 1 interleaved obrazek.pgm filterjeden.txt result.pgm
	./threads 2 interleaved obrazek.pgm filterjeden.txt result.pgm
	./threads 4 interleaved obrazek.pgm filterjeden.txt result.pgm
	./threads 8 interleaved obrazek.pgm filterjeden.txt result.pgm
	./threads 1 interleaved coins.pgm filterdwa.txt result.pgm
	./threads 2 interleaved coins.pgm filterdwa.txt result.pgm
	./threads 4 interleaved coins.pgm filterdwa.txt result.pgm
	./threads 8 interleaved coins.pgm filterdwa.txt result.pgm
	./threads 1 interleaved obrazek.pgm filterdwa.txt result.pgm
	./threads 2 interleaved obrazek.pgm filterdwa.txt result.pgm
	./threads 4 interleaved obrazek.pgm filterdwa.txt result.pgm
	./threads 8 interleaved obrazek.pgm filterdwa.txt result.pgm
	./threads 1 interleaved coins.pgm filtertrzy.txt result.pgm
	./threads 2 interleaved coins.pgm filtertrzy.txt result.pgm
	./threads 4 interleaved coins.pgm filtertrzy.txt result.pgm
	./threads 8 interleaved coins.pgm filtertrzy.txt result.pgm
	./threads 1 interleaved obrazek.pgm filtertrzy.txt result.pgm
	./threads 2 interleaved obrazek.pgm filtertrzy.txt result.pgm
	./threads 4 interleaved obrazek.pgm filtertrzy.txt result.pgm
	./threads 8 interleaved obrazek.pgm filtertrzy.txt result.pgm
