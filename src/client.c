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
#define TICK_REST 50

int client_Ip = -1;
int client_Port = -1;

game_State* client_Game;

SOCKET client_Socket;

STRINGBUFFER* client_Buffer;

bool client_Init();
bool client_Tick();
void client_Shutdown();

void client_PrintState();

int main(int argc, char** argv) {
	if(argc < 3)
		die("Usage: client <host-ip> <port>\n");

	if(!net_Init())
		die("Network init fail\n");

	client_Ip = inet_addr(argv[1]);
	client_Port = atoi(argv[2]);

	if(!client_Init())
		die("Client init fail\n");

	while(client_Tick())
		Sleep(TICK_REST);

	client_Shutdown();
	net_Shutdown();

	return 0;
}

bool client_Init() {
	client_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(client_Socket == INVALID_SOCKET)
		return 0;

	struct sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = client_Ip;
		serverAddr.sin_port = htons(client_Port);

	char on = 1;
	setsockopt(client_Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
	setsockopt(client_Socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);

	int res = connect(client_Socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(res != 0)
		return 0;

	client_Game = game_Init(1, 1);
	client_Buffer = stringbuffer_create();
	return 1;
}

bool client_Tick() {
	//Wait for sync
	console_Clear();
	client_PrintState();
	printf("Waiting for sync... \n");
	while(!net_CanRead(client_Socket))
		;

	stringbuffer_clear(client_Buffer);
	recv_stringbuffer(client_Socket, client_Buffer);
	proto_Msg* msg = proto_ParseMsg(stringbuffer_data(client_Buffer));

	if(msg->type == MSG_SYNC) {
		printf("Syncing game state... \n\t%s\n", stringbuffer_data(client_Buffer));
		game_Sync(client_Game, msg->sync.heapData, msg->sync.heapCount, msg->sync.maxItemsPerTurn);
	}
	else if (msg->type == MSG_FINISH) {
		printf("Game finished\n");
		printf("Reason: %d - ", msg->finish.result);
		switch(msg->finish.result) {
			case FIN_WIN: printf("You won!\n"); break;
			case FIN_LOSE: printf("You lost!\n"); break;
			case FIN_ERROR: printf("Error. ._. \n"); break;
			default: printf("???\n"); break;
		}

		proto_FreeMsg(msg);

		return 0;
	}
	else {
		printf("Excpected sync or finish message. Got instead: \n%s\n", stringbuffer_data(client_Buffer));

		proto_FreeMsg(msg);
		return 0;
	}

	proto_FreeMsg(msg);
	msg = NULL;

	//

	int heapId, itemCount;

	console_Clear();
	client_PrintState();
	printf("Your next move? ");

	while(1) {
		char line[64];
		fgets(line, 64, stdin);
		line[63] = '\0';

		if(strncmp(line, "surrender", strlen("surrender")) == 0) {
			msg = proto_CreateSurrenderMsg();
			char* buffer = proto_SerializeMsg(msg);

			send(client_Socket, buffer, strlen(buffer)+1, 0);

			free(buffer);
			proto_FreeMsg(msg);
			msg = NULL;

			break;
		} 
		else if(sscanf(line, "%d %d", &heapId, &itemCount) == 2) {
			msg = proto_CreateTurnMsg(heapId, itemCount);
			char* buffer = proto_SerializeMsg(msg);

			send(client_Socket, buffer, strlen(buffer)+1, 0);
			printf("Move sent, waiting for response... \n");

			proto_FreeMsg(msg);
			free(buffer);

			break;
		} 

		printf("Invalid move: %s\n", line);
	}

	//

	while(!net_CanRead(client_Socket))
		;

	stringbuffer_clear(client_Buffer);
	recv_stringbuffer(client_Socket, client_Buffer);

	printf("Received buffer: %s\n", stringbuffer_data(client_Buffer));

	msg = proto_ParseMsg(stringbuffer_data(client_Buffer));
	switch(msg->type) {
		case MSG_ACK: 
			printf("Server acknowledged with result %d\n", msg->ack.response);
		break;
	}

	proto_FreeMsg(msg);

	return true;
}

void client_Shutdown() {
	proto_Msg* msgAck = proto_CreateAckMsg(ACK_VALID);
	char* buffer = proto_SerializeMsg(msgAck);

	send(client_Socket, buffer, strlen(buffer), 0);

	free(buffer);
	proto_FreeMsg(msgAck);

	closesocket(client_Socket);
	game_Free(client_Game);
}

void client_PrintState() {
	printf("Heaps: \n");
	for(int i = 0; i < game_GetHeapCount(client_Game); i++) 
		printf("\t[%d]: %d\n", i, game_GetHeap(client_Game, i));
	printf("\n");

	printf("Max items per turn: %d\n", game_GetMaxItemsPerTurn(client_Game));
}