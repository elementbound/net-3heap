#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#define HEAP_COUNT 3

typedef struct {
	int*	heap;
	int		heapCount;
	int		maxItemsPerTurn;
} game_State;

game_State* game_Init(int itemsPerHeap, int maxItemsPerTurn) {
	game_State* game = (game_State*)malloc(sizeof(game_State));

	game->heapCount = HEAP_COUNT;
	game->heap = (int*)malloc(game->heapCount * sizeof(int));
	game->maxItemsPerTurn = maxItemsPerTurn;

	for(int i = 0; i < game->heapCount; i++) 
		game->heap[i] = itemsPerHeap;

	return game;
}

void game_Free(game_State* game) {
	free(game->heap);
	free(game);
}

void game_Sync(game_State* game, int* heapData, int heapCount, int maxItemsPerTurn) {
	free(game->heap);
	
	game->heap = (int*)malloc(heapCount * sizeof(int));
	memcpy(game->heap, heapData, heapCount * sizeof(int));

	game->heapCount = heapCount;
	game->maxItemsPerTurn = maxItemsPerTurn;
}

bool game_Turn(game_State* game, int heapId, int itemCount) {
	if(heapId >= game->heapCount) {
		printf("[Game]Turn: wrong heap %d/%d\n", heapId, game->heapCount);
		return false;
	}

	if(itemCount > game->maxItemsPerTurn) {
		printf("[Game]Turn: take over limit %d/%d\n", itemCount, game->maxItemsPerTurn);
		return false;
	}

	if(itemCount > game->heap[heapId]) {
		printf("[Game]Turn: take over heap %d/%d\n", itemCount, game->heap[heapId]);
		return false;
	}


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
const int* game_GetHeapData(game_State* game) { return game->heap; }