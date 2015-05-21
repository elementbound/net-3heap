#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define ALLOC_SIZE 512
#define STRBUFF_TAIL(strbuff) ((strbuff->size == 0) ? strbuff->data : (strbuff->data + strbuff->size - 1))

typedef struct {
	char* 	data;
	size_t 	size;
	size_t	capacity;
} STRINGBUFFER;

STRINGBUFFER* stringbuffer_create() {
	STRINGBUFFER* strbuff = (STRINGBUFFER*)malloc(sizeof(STRINGBUFFER));
	strbuff->data = NULL;
	strbuff->size = 0;
	strbuff->capacity = 0;

	return strbuff;
}

void stringbuffer_free(STRINGBUFFER* strbuff) {
	free(strbuff->data);
	free(strbuff);
}

void stringbuffer_free_shallow(STRINGBUFFER* strbuff) {
	free(strbuff);
}

int  stringbuffer_reserve(STRINGBUFFER* strbuff, size_t size) {
	if(size <= strbuff->capacity)
		return 1;

	size = (size/ALLOC_SIZE + 1)*ALLOC_SIZE;

	char* new_data = (char*)calloc(size, sizeof(char));
	if(new_data == NULL)
		return 0;

	if(size > strbuff->size) 
		memcpy(new_data, strbuff->data, strbuff->size);
	else 
		memcpy(new_data, strbuff->data, size);

	free(strbuff->data);
	strbuff->data = new_data;
	strbuff->capacity = size;
	if(size < strbuff->size) 
		strbuff->size = strbuff->capacity;

	return 1;
}

void stringbuffer_clear(STRINGBUFFER* strbuff) {
	free(strbuff->data);

	strbuff->data = NULL;
	strbuff->size = 0;
	strbuff->capacity = 0;
}

void stringbuffer_append(STRINGBUFFER* strbuff, const char* str) {
	int size = strlen(str)+1;

	stringbuffer_reserve(strbuff, strbuff->size + size);
	memcpy(STRBUFF_TAIL(strbuff), str, size);
	
	if(strbuff->size == 0)
		strbuff->size += size;
	else
		strbuff->size += size-1;
}

void stringbuffer_printf(STRINGBUFFER* strbuff, const char* format, ...) {
	va_list args;
	va_start(args, format);

	int length = vsnprintf(NULL, 0, format, args) + 1;

	stringbuffer_reserve(strbuff, strbuff->size + length);
	vsnprintf(STRBUFF_TAIL(strbuff), length, format, args);

	if(strbuff->size == 0)
		strbuff->size += length;
	else
		strbuff->size += length-1;
}

void stringbuffer_append_int(STRINGBUFFER* strbuff, 	int val) 			{ stringbuffer_printf(strbuff, "%d", val); }
void stringbuffer_append_uint(STRINGBUFFER* strbuff, 	unsigned int val)	{ stringbuffer_printf(strbuff, "%d", val); }
void stringbuffer_append_float(STRINGBUFFER* strbuff, 	float val)			{ stringbuffer_printf(strbuff, "%f", val); }
void stringbuffer_append_double(STRINGBUFFER* strbuff, 	double val)			{ stringbuffer_printf(strbuff, "%f", val); }

char*  stringbuffer_data(STRINGBUFFER* strbuff) { return strbuff->data; }
size_t stringbuffer_size(STRINGBUFFER* strbuff) { return strbuff->size; }
size_t stringbuffer_capacity(STRINGBUFFER* strbuff) { return strbuff->capacity; }