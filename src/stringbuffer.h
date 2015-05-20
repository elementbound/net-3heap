#ifndef _H_STRINGBUFFER_H_
#define _H_STRINGBUFFER_H_

#include <string.h>

typedef struct STRINGBUFFER STRINGBUFFER;

STRINGBUFFER* stringbuffer_create();
void stringbuffer_free(STRINGBUFFER* strbuff);
void stringbuffer_free_shallow(STRINGBUFFER* strbuff); //leave its string intact

int  stringbuffer_reserve(STRINGBUFFER* strbuff, size_t size);

void stringbuffer_append(STRINGBUFFER* strbuff, const char* str);
void stringbuffer_printf(STRINGBUFFER* strbuff, const char* format, ...);

void stringbuffer_append_int(STRINGBUFFER* strbuff, 	int val);
void stringbuffer_append_uint(STRINGBUFFER* strbuff, 	unsigned int val);
void stringbuffer_append_float(STRINGBUFFER* strbuff, 	float val);
void stringbuffer_append_double(STRINGBUFFER* strbuff, 	double val);

char*  stringbuffer_data(STRINGBUFFER* strbuff);
size_t stringbuffer_size(STRINGBUFFER* strbuff);
size_t stringbuffer_capacity(STRINGBUFFER* strbuff);

#endif