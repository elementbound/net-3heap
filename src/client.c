#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "net.h"
#include "protocol.h"
#include "game.h"

#define die(msg) {printf(msg); exit(1);}

int client_Ip = -1;
int client_Port = -1;

game_State* client_Game;

SOCKET client_Socket;

STRINGBUFFER* client_Buffer;

bool client_Init();
bool client_Tick();
void client_Shutdown();

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
		;

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

	int res = connect(client_Socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(res != 0)
		return 0;

	client_Game = game_Init(1, 1);
	client_Buffer = stringbuffer_create();
	return 1;
}

bool client_Tick() {
	int heapId, itemCount;

	printf("Your next move? ");
	scanf("%d %d", &heapId, &itemCount);

	proto_Msg* msg = proto_CreateTurnMsg(heapId, itemCount);
	char* buffer = proto_SerializeMsg(msg);

	send(client_Socket, buffer, strlen(buffer)+1, 0);
	printf("Move sent, waiting for response... \n");

	proto_FreeMsg(msg);
	free(buffer);

	//

	while(!net_CanRead(client_Socket))
		;

	stringbuffer_clear(client_Buffer);
	recv_stringbuffer(client_Socket, client_Buffer);

	msg = proto_ParseMsg(stringbuffer_data(client_Buffer));
	switch(msg->type) {
		case MSG_ACK: 
			printf("Server acknowledged with result %d\n", msg->ack.response);
		break;

		case MSG_FINISH: 
			printf("Game ended\n");
		break;

		default: 
			printf("%s\n", stringbuffer_data(client_Buffer));
	}

	proto_FreeMsg(msg);

	return true;
}

void client_Shutdown() {
	closesocket(client_Socket);
	game_Free(client_Game);
}