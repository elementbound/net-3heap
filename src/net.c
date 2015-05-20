#include <winsock2.h>
#include <ws2tcpip.h>
#include "net.h"

bool net_Init() {
	WSADATA wsaData;
	
	int res;
	if(res = WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
		return 0;

	return 1;
}

void net_Shutdown() {
	WSACleanup();
}

#define TIMEOUT_USEC 10

int recv_stringbuffer(SOCKET sock, STRINGBUFFER* strbuff) {
	char buffer[65];
	int	recvdTotal = 0;
	int recvd;

	struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = TIMEOUT_USEC;

	fd_set read_set;
		read_set.fd_count = 1;
		read_set.fd_array[0] = sock;

	while(select(1, &read_set, NULL, NULL, &timeout) > 0) {
		recvd = recv(sock, buffer, 64, 0);
		if(recvd < 0)
			break;

		buffer[recvd] = '\0';
		stringbuffer_append(strbuff, buffer);
		recvdTotal += recvd;
	}

	return recvdTotal;
}