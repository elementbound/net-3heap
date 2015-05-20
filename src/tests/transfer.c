#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define PORT 49153

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
				printf("Bind fail: %d\n", res);
				return 1;
			}
	#endif

	#ifdef CLIENT
	#endif

	return 0;
}