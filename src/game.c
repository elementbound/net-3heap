#include <stdbool.h>
#include <stdlib.h>

#define HEAP_COUNT 3

typedef struct {
	int heap[HEAP_COUNT];
	int maxItemsPerTurn;
} game_State;

game_State* game_Init(int itemsPerHeap, int maxItemsPerTurn) {
	game_State* game = (game_State*)malloc(sizeof(game_State));

	for(int i = 0; i < HEAP_COUNT; i++)
		game->heap[i] = 0;
	game->maxItemsPerTurn = maxItemsPerTurn;

	return game;
}

void game_Free(game_State* game) {
	free(game);
}

bool game_Turn(game_State* game, int heapId, int itemCount) {
	if(heapId >= HEAP_COUNT) 
		return false;

	if(itemCount > game->maxItemsPerTurn)
		return false;

	if(itemCount > game->heap[heapId])
		return false;

	game->heap[heapId] -= itemCount;
	return true;
}

bool game_IsFinished(game_State* game) {
	for(int i = 0; i < HEAP_COUNT; i++) 
		if(game->heap[i] != 0)
			return false;

	return true;
}

int game_GetHeapCount(game_State* game) { return HEAP_COUNT; }
int game_GetHeap(game_State* game, int heapId) { return (heapId < HEAP_COUNT) ? game->heap[heapId] : -1; }
int game_GetMaxItemsPerTurn(game_State* game) { return game->maxItemsPerTurn; }