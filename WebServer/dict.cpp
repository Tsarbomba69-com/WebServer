#include "dict.h"

Pair pair_constructor(char* key, void* value, unsigned long size)
{
	if (size < 1)
	{
		// Confirm the size of the data is at least one, otherwise exit with an error message.
		printf("Invalid data size for node...\n");
		exit(1);
	}
	// Create a Node instance to be returned
	Pair node;
	// Allocate space for the data if it is of a supported type
	node.value = value;
	node.key = (char*)malloc(strlen(key));
	strcpy_s(node.key, strlen(key) + 1, key);
	// Initialize the pointers.
	node.next = NULL;
	return node;
}

void pair_destructor(Pair* node)
{
	free(node->value);
	free(node);
}

Dictionary dict_constructor()
{
	Dictionary dict;
	dict.head = NULL;
	dict.length = 0;
	return dict;
}

Pair* create_pair(char* key, void* value, unsigned long size)
{
	// Allocate space.
	Pair* new_node = (Pair*)malloc(sizeof(Pair));
	// Call the constructor.
	*new_node = pair_constructor(key, value, size);
	return new_node;
}

void insert_item(Dictionary* dict, char* key, void* value, unsigned long size) {
	// Create a new node to be inserted.
	Pair* node_to_insert = create_pair(key, value, size);

	if (dict->length == 0)
	{
		dict->head = node_to_insert;
	}
	else
	{
		Pair* cursor = dict->head;
		// Step through the list until the desired index is reached.
		do {
			if (cursor->next == NULL) {
				cursor->next = node_to_insert;
				break;
			}
		} while (cursor = cursor->next);
	}
	// Increment the list length.
	dict->length += 1;
}

Pair* search_dict(Dictionary* dict, char* key)
{
	// Confirm the user has specified a valid index.
	if (dict->length == 0)
	{
		return NULL;
	}
	// Create a cursor node for iteration.
	Pair* cursor = dict->head;
	// Step through the list until the desired index is reached.
	do {
		if (strcmp(cursor->key, key) == 0) {
			return cursor;
		}
	} while (cursor = cursor->next);
	return NULL;
}

void* get_item(Dictionary* dict, char* key) {
	Pair* node = search_dict(dict, key);
	if (node) {
		return node->value;
	}
	else {
		return NULL;
	}
}