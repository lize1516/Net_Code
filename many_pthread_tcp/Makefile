.PHONY:all
all:client server

client:client_tcp.c
	gcc -o $@ $^
server:server_tcp.c
	gcc -o $@ $^ -lpthread

.PHONY:clean
clean:
	rm client server
