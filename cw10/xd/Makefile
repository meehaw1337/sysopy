CC = gcc -Wall

all: server client


server:
	$(CC) server.c message.h -o server -lpthread


client:
	$(CC) client.c message.h -o client -lpthread


clean:
	rm -rf client server
