#include <winsock2.h>
#include <ws2tcpip.h>
#include "net.h"

#include <stdio.h>

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

bool net_CanRead(SOCKET sock) {
	struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = TIMEOUT_USEC;

	fd_set readSet;
		readSet.fd_count = 1;
		readSet.fd_array[0] = sock;

	return (select(1, &readSet, NULL, NULL, &timeout) > 0);
}

bool net_CanWrite(SOCKET sock) {
	struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = TIMEOUT_USEC;

	fd_set writeSet;
		writeSet.fd_count = 1;
		writeSet.fd_array[0] = sock;

	return (select(1, NULL, &writeSet, NULL, &timeout) > 0);
}

int recv_stringbuffer(SOCKET sock, STRINGBUFFER* strbuff) {
	char buffer[65];
	int	recvdTotal = 0;
	int recvd;

	//printf("[recv]Receive loop\n");
	while(net_CanRead(sock)) {
		recvd = recv(sock, buffer, 64, 0);
		if(recvd < 0)
			break;

		buffer[recvd] = '\0';
		stringbuffer_append(strbuff, buffer);
		recvdTotal += recvd;

		//printf("\tReceived %d bytes: %s\n", recvd, buffer);
	}

	//printf("\tFull message: %s\n", stringbuffer_data(strbuff));
	//printf("\tMessage size: %d/%d\n", recvdTotal, stringbuffer_size(strbuff));
	return recvdTotal;
}

int send_stringbuffer(SOCKET sock, STRINGBUFFER* strbuff) {
	return send(sock, stringbuffer_data(strbuff), stringbuffer_size(strbuff), 0);
}