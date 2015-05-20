#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <string.h>

#include "net.h"
#include "protocol.h"
#include "game.h"

#define die(msg) {printf(msg); exit(1);}

#define PLAYER_COUNT 2

int server_Port = 65535;
int server_HeapMax = 3;

game_State* server_Game = NULL;

bool server_Init();
void server_AcceptPlayer(int playerId);
bool server_Tick();
void server_Shutdown();

int main(int argc, char** argv) {
	if(argc < 3)
		die("Usage: server <port> <heapMax>\n");

	server_Port = atoi(argv[1]);
	server_HeapMax = atoi(argv[2]);

	if(!net_Init())
		die("Network init fail\n");

	if(!server_Init())
		die("Server init fail\n");

	for(int i = 0; i < PLAYER_COUNT; i++)
		server_AcceptPlayer(i);

	while(server_Tick())
		;

	server_Shutdown();
	net_Shutdown();
	return 0;
}