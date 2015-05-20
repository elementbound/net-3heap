#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "net.h"
#include "protocol.h"
#include "game.h"

#define die(msg) {printf(msg); exit(1);}

#define PLAYER_COUNT 2

int server_Port = 65535;
int server_HeapMax = 3;
int server_TakeMax = 2;

game_State* server_Game = NULL;
int 		server_NextPlayer = -1;

SOCKET server_ListenSocket;
SOCKET server_PlayerSockets[PLAYER_COUNT];

bool server_Init();
bool server_AcceptPlayer(int playerId);
bool server_Tick();
void server_Shutdown();

int main(int argc, char** argv) {
	if(argc < 4)
		die("Usage: server <port> <heapMax> <takeMax>\n");

	server_Port = atoi(argv[1]);
	server_HeapMax = atoi(argv[2]);
	server_TakeMax = atoi(argv[3]);

	if(!net_Init())
		die("Network init fail\n");

	if(!server_Init())
		die("Server init fail\n");

	for(int i = 0; i < PLAYER_COUNT; i++)
		while(!server_AcceptPlayer(i))
			;

	while(server_Tick())
		;

	server_Shutdown();
	net_Shutdown();
	return 0;
}

bool server_Init() {
	server_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(server_ListenSocket == INVALID_SOCKET)
		return 0;

	struct sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_port = htons(server_Port);

	int res = bind(server_ListenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(res != 0)
		return 0;

	server_Game = game_Init(server_HeapMax, server_TakeMax);
	server_NextPlayer = 0;
	return 1;
}

bool server_AcceptPlayer(int playerId) {
	int res = listen(server_ListenSocket, SOMAXCONN);
	if(res != 0)
		return 0;

	server_PlayerSockets[playerId] = accept(server_ListenSocket, NULL, NULL);
	if(server_PlayerSockets[playerId] == INVALID_SOCKET)
		return 0;

	return 1;
}

void server_Shutdown() {
	closesocket(server_ListenSocket);

	for(int i = 0; i < PLAYER_COUNT; i++)
		closesocket(server_PlayerSockets[i]);

	game_Free(server_Game);
}