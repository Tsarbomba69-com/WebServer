#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Dict {
	char* key;
	void* value;
	Dict* next = NULL;
} Dict;

typedef struct Pair {
	char* key;
	void* value;
	Pair* next;
} Pair;

typedef struct Dictionary {
	Pair* head;
	// Length refers to the number of nodes in the chain.
	int length;
} Dictionary;

Dictionary dict_constructor(void);

Pair pair_constructor(char* key, void* value, unsigned long size);

void insert_item(Dictionary* dict, char* key, void* value, unsigned long size);

Pair* search_dict(Dictionary* dict, char* key);

void* get_item(Dictionary* dict, char* key);