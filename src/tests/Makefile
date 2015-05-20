build: server client

server:
	gcc -std=c99 -DSERVER transfer.c -lws2_32 -o transfer-server

client:
	gcc -std=c99 -DCLIENT transfer.c -lws2_32 -o transfer-client