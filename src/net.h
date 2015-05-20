#ifndef _H_NET_H_
#define _H_NET_H_

#include <stdbool.h>
#include "stringbuffer.h"

bool net_Init();
void net_Shutdown();

int recv_stringbuffer(SOCKET sock, STRINGBUFFER* strbuff);

#endif