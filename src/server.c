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

STRINGBUFFER* server_Buffer; 

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
	server_Buffer = stringbuffer_create();
	return 1;
}

bool server_AcceptPlayer(int playerId) {
	int res = listen(server_ListenSocket, SOMAXCONN);
	if(res != 0)
		return 0;

	server_PlayerSockets[playerId] = accept(server_ListenSocket, NULL, NULL);
	if(server_PlayerSockets[playerId] == INVALID_SOCKET)
		return 0;

	stringbuffer_clear(server_Buffer);
	return 1;
}

bool server_Tick() {
	if(!net_CanWrite(server_PlayerSockets[server_NextPlayer]))
		return 0;

	int waitCounter;
	for(waitCounter = 0; !net_CanRead(server_PlayerSockets[server_NextPlayer]); waitCounter++)
		printf("Waiting for player[%d]... %d\r", server_NextPlayer, waitCounter);

	recv_stringbuffer(server_PlayerSockets[server_NextPlayer], server_Buffer);
	proto_Msg* msg = proto_ParseMsg(stringbuffer_data(server_Buffer));
	proto_Msg* msgResponse = NULL;

	switch(msg->type) {
		case MSG_TURN:
			if(game_Turn(server_Game, msg->turn.heapId, msg->turn.itemCount)) 
				msgResponse = proto_CreateAckMsg(ACK_VALID);
			else
				msgResponse = proto_CreateAckMsg(ACK_INVALID);
		break;

		case MSG_SURRENDER:
			msgResponse = proto_CreateAckMsg(ACK_VALID);
		break;

		default: 
			msgResponse = proto_CreateAckMsg(ACK_INVALID);
	}

	char* msgBuffer = proto_SerializeMsg(msgResponse);
	send(server_PlayerSockets[server_NextPlayer], msgBuffer, strlen(msgBuffer)+1, 0);

	free(msgBuffer);
	proto_FreeMsg(msg);
	proto_FreeMsg(msgResponse);

	return 1;
}

void server_Shutdown() {
	closesocket(server_ListenSocket);

	for(int i = 0; i < PLAYER_COUNT; i++)
		closesocket(server_PlayerSockets[i]);

	game_Free(server_Game);
}