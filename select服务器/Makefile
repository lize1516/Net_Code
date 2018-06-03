.PHONY:all
all:server client
server:server_select.c
	gcc $^ -o $@
client:client_select.c
	gcc $^ -o $@
.PHONY:clean
clean:
	rm server client

