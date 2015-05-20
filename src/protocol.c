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

//



//

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