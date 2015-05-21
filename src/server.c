#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "net.h"
#include "protocol.h"
#include "game.h"
#include "console.h"

#define die(msg) {printf(msg); exit(1);}

#define PLAYER_COUNT 2

int server_Port = 65535;
int server_HeapMax = 3;
int server_TakeMax = 2;

game_State* server_Game = NULL;
int 		server_NextPlayer = -1;
int			server_PlayerResult[PLAYER_COUNT];

SOCKET server_ListenSocket;
SOCKET server_PlayerSockets[PLAYER_COUNT];

STRINGBUFFER* server_Buffer; 

bool server_Init();
bool server_AcceptPlayer(int playerId);
bool server_Tick();
void server_Shutdown();

void server_Sync(int playerId);
void server_PrintState();

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

	for(int i = 0; i < PLAYER_COUNT; i++) {
		printf("Waiting for player %d... \n", i);
		while(!server_AcceptPlayer(i))
			;
	}

	while(server_Tick())
		;//Sleep(2000);

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
	if(!net_CanWrite(server_PlayerSockets[server_NextPlayer])) {
		printf("Can't send data to player %d\n", server_NextPlayer);
		for(int i = 0; i <PLAYER_COUNT; i++)
			server_PlayerResult[i] = FIN_ERROR;

		return 0;
	}

	//sync
	console_Clear();
	server_PrintState();
	printf("Syncing... \n");
	server_Sync(server_NextPlayer);

	//wait for player turn
	console_Clear();
	server_PrintState();
	printf("Waiting for player... \n");
	while(!net_CanRead(server_PlayerSockets[server_NextPlayer]))
		;

	stringbuffer_clear(server_Buffer);
	recv_stringbuffer(server_PlayerSockets[server_NextPlayer], server_Buffer);
	printf("Received buffer: %s\n", stringbuffer_data(server_Buffer));

	proto_Msg* msg = proto_ParseMsg(stringbuffer_data(server_Buffer));
	proto_Msg* msgResponse = NULL;
	bool validMove = 0;

	switch(msg->type) {
		case MSG_TURN:
			printf("Got a turn: Take %d from %d\n", msg->turn.itemCount, msg->turn.heapId);

			if(game_Turn(server_Game, msg->turn.heapId, msg->turn.itemCount)) {
				msgResponse = proto_CreateAckMsg(ACK_VALID);
				validMove = 1;
			}
			else
				msgResponse = proto_CreateAckMsg(ACK_INVALID);
		break;

		case MSG_SURRENDER:
			msgResponse = proto_CreateAckMsg(ACK_VALID);
			validMove = 1;
		break;

		default: 
			msgResponse = proto_CreateAckMsg(ACK_INVALID);
	}

	char* msgBuffer = proto_SerializeMsg(msgResponse);
	send(server_PlayerSockets[server_NextPlayer], msgBuffer, strlen(msgBuffer)+1, 0);

	free(msgBuffer);
	proto_FreeMsg(msg);
	proto_FreeMsg(msgResponse);

	//

	if(validMove) {
		if(game_IsFinished(server_Game)) {
			printf("Game finished\n");

			for(int i = 0; i < PLAYER_COUNT; i++)
				server_PlayerResult[i] = (i == server_NextPlayer) ? FIN_WIN : FIN_LOSE;

			return 0;
		}

		server_NextPlayer = (server_NextPlayer+1) % PLAYER_COUNT;
	}

	return 1;
}

void server_Shutdown() {
	printf("Server shutdown!\n");

	printf("\tClosing listen socket...\n");
	closesocket(server_ListenSocket);

	for(int i = 0; i < PLAYER_COUNT; i++) {
		printf("\tParting ways with player %d\n", i);
		Sleep(500);
		
		printf("\t\tSending finish message\n", i);
		proto_Msg* msg = proto_CreateFinishMsg(server_PlayerResult[i]);
		char* buffer = proto_SerializeMsg(msg);
		send(server_PlayerSockets[i], buffer, strlen(buffer)+1, 0);

		free(buffer);
		proto_FreeMsg(msg);

		printf("\t\tClosing socket\n");
		closesocket(server_PlayerSockets[i]);

		printf("\n");
	}

	game_Free(server_Game);
}

void server_PrintState() {
	printf("Heaps: \n");
	for(int i = 0; i < game_GetHeapCount(server_Game); i++) 
		printf("\t[%d]: %d\n", i, game_GetHeap(server_Game, i));

	printf("\n");
	printf("Next player: %d\n", server_NextPlayer);
	printf("Max items per turn: %d\n", server_TakeMax);
	printf("Initial heap size: %d\n", server_HeapMax);

	printf("\n");
}

void server_Sync(int playerId) {
	proto_Msg* msg = proto_CreateSyncMsg(game_GetHeapCount(server_Game), game_GetHeapData(server_Game));
	char* buffer = proto_SerializeMsg(msg);

	send(server_PlayerSockets[playerId], buffer, strlen(buffer)+1, 0);

	free(buffer);
	proto_FreeMsg(msg);
}