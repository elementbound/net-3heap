#include "protocol.h"
#include "stringbuffer.h"

#include <stdlib.h>
#include <string.h>

proto_Msg* proto_AllocMsg() {
	proto_Msg* msg = (proto_Msg*)malloc(sizeof(proto_Msg));

	msg->type = MSG_UNKNOWN;
	msg->sync.heapData = NULL;

	return msg;
}

proto_Msg* proto_CreateSyncMsg(int heapCount, const int* heapData) {
	proto_Msg* msg = proto_AllocMsg();

	msg->type = MSG_SYNC;
	msg->sync.heapCount = heapCount;
	msg->sync.heapData = (int*)malloc(sizeof(int)*heapCount);
		memcpy(msg->sync.heapData, heapData, sizeof(int)*heapCount);

	return msg;
}

proto_Msg* proto_CreateTurnMsg(int heapId, int itemCount) {
	proto_Msg* msg = proto_AllocMsg();

	msg->type = MSG_TURN;
	msg->turn.heapId = heapId;
	msg->turn.itemCount = itemCount;

	return msg;
}

proto_Msg* proto_CreateSurrenderMsg() {
	proto_Msg* msg = proto_AllocMsg();

	msg->type = MSG_SURRENDER;
	return msg;
}

proto_Msg* proto_CreateFinishMsg() {
	proto_Msg* msg = proto_AllocMsg();

	msg->type = MSG_FINISH;
	return msg;
}

proto_Msg* proto_CreateAckMsg(int response) {
	proto_Msg* msg = proto_AllocMsg();

	msg->type = MSG_ACK;
	msg->ack.response = response;

	return msg;
}

void proto_FreeMsg(proto_Msg* msg) {
	free(msg->sync.heapData);
	free(msg);
}

//===========================================================================================//
//Message parsers

proto_Msg* proto_ParseSync(const char* buffer) {
	proto_Msg* msg = proto_AllocMsg();
	msg->type = MSG_SYNC;

	char* str = (char*)calloc(strlen(buffer)+1, sizeof(char));
	char* token = NULL;
	strcpy(str, buffer);
	
	token = strtok(str, "|");
	//Skip type

	token = strtok(NULL, "|");
	msg->sync.maxItemsPerTurn = atoi(token);
	token = strtok(NULL, "|");
	msg->sync.heapCount = atoi(token);
	msg->sync.heapData = (int*)malloc(sizeof(int) * msg->sync.heapCount);

	for(int i = 0; token = strtok(NULL, "|"); i++) 
		msg->sync.heapData[i] = atoi(token);

	free(str);
	return msg;
}

proto_Msg* proto_ParseTurn(const char* buffer){
	proto_Msg* msg = proto_AllocMsg();
	msg->type = MSG_TURN;

	char* str = (char*)calloc(strlen(buffer)+1, sizeof(char));
	strcpy(str, buffer);
	sscanf(strchr(str, '|')+1, "%d|%d", &msg->turn.heapId, &msg->turn.itemCount);

	free(str);
	return msg;
}

proto_Msg* proto_ParseSurrender(const char* buffer) {
	proto_Msg* msg = proto_AllocMsg();
	msg->type = MSG_SURRENDER;

	return msg;
}

proto_Msg* proto_ParseFinish(const char* buffer) {
	proto_Msg* msg = proto_AllocMsg();
	msg->type = MSG_FINISH;
	
	return msg;
} 

proto_Msg* proto_ParseAck(const char* buffer) {
	proto_Msg* msg = proto_AllocMsg();
	msg->type = MSG_ACK;
	msg->ack.response = atoi(strchr(buffer, '|')+1);
	
	return msg;
}

//===========================================================================================//
//Message serializers

char* proto_SerializeSync(proto_Msg* msg) {
	STRINGBUFFER* strbuff = stringbuffer_create();
	char* str;

	stringbuffer_printf(strbuff, "%s|%d|%d", MSGSTR_SYNC, msg->sync.maxItemsPerTurn, msg->sync.heapCount);
	for(int i=0; i < msg->sync.heapCount; i++)
		stringbuffer_printf(strbuff, "|%d", msg->sync.heapData[i]);

	str = stringbuffer_data(strbuff);
	stringbuffer_free_shallow(strbuff);
	return str;
}

char* proto_SerializeTurn(proto_Msg* msg) {
	STRINGBUFFER* strbuff = stringbuffer_create();
	char* str;
	
	stringbuffer_printf(strbuff, "%s|%d|%d", MSGSTR_TURN, msg->turn.heapId, msg->turn.itemCount);

	str = stringbuffer_data(strbuff);
	stringbuffer_free_shallow(strbuff);
	return str;
}

char* proto_SerializeSurrender(proto_Msg* msg) {
	STRINGBUFFER* strbuff = stringbuffer_create();
	char* str;
	
	stringbuffer_printf(strbuff, "%s|", MSGSTR_SURRENDER);

	str = stringbuffer_data(strbuff);
	stringbuffer_free_shallow(strbuff);
	return str;
}

char* proto_SerializeFinish(proto_Msg* msg) {
	STRINGBUFFER* strbuff = stringbuffer_create();
	char* str;
	
	stringbuffer_printf(strbuff, "%s|", MSGSTR_FINISH);

	str = stringbuffer_data(strbuff);
	stringbuffer_free_shallow(strbuff);
	return str;
}

char* proto_SerializeAck(proto_Msg* msg) {
	STRINGBUFFER* strbuff = stringbuffer_create();
	char* str;
	
	stringbuffer_printf(strbuff, "%s|%d", MSGSTR_ACK, msg->ack.response);

	str = stringbuffer_data(strbuff);
	stringbuffer_free_shallow(strbuff);
	return str;
} 

//===========================================================================================//

proto_Msg* proto_ParseMsg(const char* buffer) {
	proto_Msg* msg = NULL;

	char* type;
	int typeLength = strchr(buffer, '|') - type;

	type = (char*)calloc(typeLength, sizeof(char));
	strncpy(type, buffer, typeLength-1);

	if(strcmp(type, MSGSTR_SYNC) == 0) 
		msg = proto_ParseSync(buffer);
	else if(strcmp(type, MSGSTR_TURN) == 0)
		msg = proto_ParseTurn(buffer);
	else if(strcmp(type, MSGSTR_SURRENDER) == 0)
		msg = proto_ParseSurrender(buffer);
	else if(strcmp(type, MSGSTR_FINISH) == 0)
		msg = proto_ParseFinish(buffer);
	else if(strcmp(type, MSGSTR_ACK) == 0)
		msg = proto_ParseAck(buffer);

	free(type);
	return msg;
}

char* proto_SerializeMsg(proto_Msg* msg) {
	switch(msg->type) {
		case MSG_SYNC: 
			return proto_SerializeSync(msg);
			break;

		case MSG_TURN:
			return proto_SerializeTurn(msg);
			break;

		case MSG_SURRENDER:
			return proto_SerializeSurrender(msg);
			break;

		case MSG_FINISH:
			return proto_SerializeFinish(msg);
			break;

		case MSG_ACK:
			return proto_SerializeFinish(msg);
			break;

		default:
			return NULL;
	}
}