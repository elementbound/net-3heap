#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define ALLOC_SIZE 512

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

int  stringbuffer_reserve(STRINGBUFFER* strbuff, size_t size) {
	if(size <= strbuff->capacity)
		return 1;

	size = (size/ALLOC_SIZE + 1)*ALLOC_SIZE;

	char* new_data = (char*)malloc(size);
	if(new_data == NULL)
		return 0;

	if(size > strbuff->size) {
		memcpy(new_data, strbuff->data, strbuff->size);
		memset(new_data+strbuff->size, 0, size-strbuff->size);
	}
	else 
		memcpy(new_data, strbuff->data, size);

	free(strbuff->data);
	strbuff->data = new_data;
	strbuff->capacity = size;

	return 1;
}

void stringbuffer_append(STRINGBUFFER* strbuff, const char* str) {
	int size = strlen(str)+1;

	stringbuffer_reserve(strbuff, strbuff->size + size);
	memcpy(strbuff->data + strbuff->size, str, size);
	strbuff->size += size-1;
}

void stringbuffer_printf(STRINGBUFFER* strbuff, const char* format, ...) {
	va_list args;
	va_start(args, format);

	int length = vsnprintf(NULL, 0, format, args);

	stringbuffer_reserve(strbuff, strbuff->size + length);
	vsnprintf(strbuff->data + strbuff->size, strbuff->capacity - strbuff->size, format, args);

	strbuff->size += length; 
}

void stringbuffer_append_int(STRINGBUFFER* strbuff, 	int val) 			{ stringbuffer_printf(strbuff, "%d", val); }
void stringbuffer_append_uint(STRINGBUFFER* strbuff, 	unsigned int val)	{ stringbuffer_printf(strbuff, "%d", val); }
void stringbuffer_append_float(STRINGBUFFER* strbuff, 	float val)			{ stringbuffer_printf(strbuff, "%f", val); }
void stringbuffer_append_double(STRINGBUFFER* strbuff, 	double val)			{ stringbuffer_printf(strbuff, "%f", val); }

char*  stringbuffer_data(STRINGBUFFER* strbuff) { return strbuff->data; }
size_t stringbuffer_size(STRINGBUFFER* strbuff) { return strbuff->size; }
size_t stringbuffer_capacity(STRINGBUFFER* strbuff) { return strbuff->capacity; }