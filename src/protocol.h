#ifndef _H_PROTOCOL_H_
#define _H_PROTOCOL_H_

#define MSGSTR_SYNC 		"SYNC"
#define MSGSTR_TURN 		"TURN"
#define MSGSTR_SURRENDER	"SURRENDER"
#define MSGSTR_FINISH		"FINISH"
#define MSGSTR_ACK			"ACK"

#define MSG_UNKNOWN			0
#define MSG_SYNC			1
#define MSG_TURN			2
#define MSG_SURRENDER		3
#define MSG_FINISH			4
#define MSG_ACK				5

#define ACK_VALID			1
#define ACK_INVALID			0

#define FIN_ERROR			1
#define FIN_WIN				2
#define FIN_LOSE			3

typedef struct {
	union {
		struct {
			int  heapCount;
			int* heapData;
			int	 maxItemsPerTurn;
		} sync;

		struct {
			int heapId;
			int itemCount;
		} turn;

		struct {
			int response;
		} ack;

		struct {
			int result;
		} finish;
	};

	int type;
} proto_Msg;

proto_Msg* proto_CreateSyncMsg(int heapCount, const int* heapData, int maxItemsPerTurn);
proto_Msg* proto_CreateTurnMsg(int heapId, int itemCount);
proto_Msg* proto_CreateSurrenderMsg();
proto_Msg* proto_CreateFinishMsg(int result);
proto_Msg* proto_CreateAckMsg(int response);

void proto_FreeMsg(proto_Msg* msg);

proto_Msg* proto_ParseMsg(const char* buffer);
char* proto_SerializeMsg(proto_Msg* msg);

#endif