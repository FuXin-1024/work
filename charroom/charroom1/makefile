.PHONY:all
all:server client
server:server.c
	gcc -o server server.c -l pthread
client:client.c
	gcc -o client client.c -l pthread
.PHONY:clean
clean:
	rm -f server client


