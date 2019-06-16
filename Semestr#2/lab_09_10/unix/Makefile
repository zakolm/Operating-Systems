all: client server

client: client.c info.h
	gcc -g client.c -o client

server: server.c info.h
	gcc -g server.c -o server
clean:
	rm client server
