#ifndef _H_3HEAPS_H_
#define _H_3HEAPS_H_

#include <stdbool.h>

typedef struct game_State game_State;

game_State* game_Init(int itemsPerHeap, int maxItemsPerTurn);
void		game_Free(game_State* game);

bool		game_Turn(game_State* game, int heapId, int itemCount);
void		game_Sync(game_State* game, int* heapData, int heapCount, int maxItemsPerTurn);

bool		game_IsFinished(game_State* game);

int 		game_GetHeapCount(game_State* game);
int 		game_GetHeap(game_State* game, int heapId);
int 		game_GetMaxItemsPerTurn(game_State* game);
const int*	game_GetHeapData(game_State* game);

#endif