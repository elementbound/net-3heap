#include <stdbool.h>
#include <stdlib.h>

#define HEAP_COUNT 3

typedef struct {
	int*	heap;
	int		heapCount;
	int		maxItemsPerTurn;
} game_State;

game_State* game_Init(int itemsPerHeap, int maxItemsPerTurn) {
	game_State* game = (game_State*)malloc(sizeof(game_State));

	game->heapCount = HEAP_COUNT;
	game->heap = (int*)calloc(game->heapCount, sizeof(int));
	game->maxItemsPerTurn = maxItemsPerTurn;

	return game;
}

void game_Free(game_State* game) {
	free(game->heap);
	free(game);
}

bool game_Turn(game_State* game, int heapId, int itemCount) {
	if(heapId >= game->heapCount) 
		return false;

	if(itemCount > game->maxItemsPerTurn)
		return false;

	if(itemCount > game->heap[heapId])
		return false;

	game->heap[heapId] -= itemCount;
	return true;
}

bool game_IsFinished(game_State* game) {
	for(int i = 0; i < game->heapCount; i++) 
		if(game->heap[i] != 0)
			return false;

	return true;
}

int game_GetHeapCount(game_State* game) { return game->heapCount; }
int game_GetHeap(game_State* game, int heapId) { return (heapId < game->heapCount) ? game->heap[heapId] : -1; }
int game_GetMaxItemsPerTurn(game_State* game) { return game->maxItemsPerTurn; }