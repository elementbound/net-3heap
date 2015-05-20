#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

#define PORT 		65535

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

		char message[64];
		printf("Waiting for data... \n");
		res = recv(clientSocket, message, 64, 0);
		printf("Received %d bytes: \n\t%s\n", res, message);

		printf("Sending echo... ");
		res = send(clientSocket, message, strlen(message)+1, 0);
		printf("sent %d bytes\n", res);
		
		printf("Waiting for client to terminate... \n");
		while(recv(clientSocket, message, 64, 0) > 0)
			;

		printf("Done, terminating\n");
		closesocket(clientSocket);
		WSACleanup();
	#endif

	#ifdef CLIENT
		SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(serverSocket == INVALID_SOCKET) {
			printf("Couldn't create socket\n");
			WSACleanup();
			return 1;
		}

		struct sockaddr_in serverAddr;
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
			serverAddr.sin_port = htons(PORT);

		res = connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
		if(res != 0) {
			printf("Connection failed: %d | %d\n", res, WSAGetLastError());
			WSACleanup();
			return 1;
		}

		printf("Connection established!\n");
		
		char message[64];
		printf("Message: ");
		fgets(message, 64, stdin);
		res = send(serverSocket, message, strlen(message)+1, 0);
		printf("Sent %d/%d bytes\n", res, strlen(message)+1);

		printf("Waiting for echo... \n");
		res = recv(serverSocket, message, 64, 0);
		printf("Received %d bytes: \n\t%s\n", res, message);

		printf("Terminating... \n");
		closesocket(serverSocket);
		WSACleanup();
	#endif

	return 0;
}