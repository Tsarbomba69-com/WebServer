#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Dict {
	char* key;
	void* value;
	Dict* next;
} Dict;


Dict** dictAlloc(void);

void addItem(Dict** dict, char* key, void* value);

void delItem(Dict** dict, char* key);

void* getItem(Dict* dict, char* key);