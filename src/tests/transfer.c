#define _WIN32_WINNT 0x501 //Why even? 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

#define PORT 		65535
#define PORT_STR	"65535"

#if !defined(CLIENT) && !defined(SERVER)
	#error Neither CLIENT or SERVER are specified
#endif

int main() 
{
	WSADATA wsaData;
	
	int res;
	if(res = WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		printf("Winsock startup fail: %d\n", res);
		return 1;
	}

	#ifdef SERVER
		SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(listenSocket == INVALID_SOCKET) {
			printf("Couldn't create socket\n");
			return 1;
		}

		struct sockaddr_in serverAddr;
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_addr.s_addr = INADDR_ANY;
			serverAddr.sin_port = htons(PORT);

		res = bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
		if(res != 0) {
			printf("Bind fail: %d - %d\n", res, WSAGetLastError());
			WSACleanup();
			return 1;
		}

		printf("Listening on port %d... ", PORT);
		res = listen(listenSocket, SOMAXCONN);
		if(res != 0) {
			printf("fail#$d\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		SOCKET clientSocket = accept(listenSocket, NULL, NULL);
		if(clientSocket == INVALID_SOCKET) {
			printf("fail#%d\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}
		printf("success\n");
	#endif

	#ifdef CLIENT
	#endif

	return 0;
}