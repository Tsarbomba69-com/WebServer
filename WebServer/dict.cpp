#include "dict.h"

Dict** dictAlloc(void) {
	return (Dict**)malloc(sizeof(Dict));
}

void addItem(Dict** dict, char* key, void* value) {
	//delItem(dict, key); /* If we already have a item with this key, delete it. */
	Dict* d = (Dict*)malloc(sizeof(Dict));
	d->key = (char*)malloc(strlen(key) + 1);
	strcpy(d->key, key);
	d->value = value;
	d->next = *dict;
	*dict = d;
}

void* getItem(Dict* dict, char* key) {
	for (Dict* ptr = dict; ptr != NULL; ptr = ptr->next) {
		if (strcmp(ptr->key, key) == 0) {
			return ptr->value;
		}
	}

	return NULL;
}

void delItem(Dict** dict, char* key) {
	Dict* ptr, * prev;
	for (ptr = *dict, prev = NULL; ptr != NULL; prev = ptr, ptr = ptr->next) {
		if (strcmp(ptr->key, key) == 0) {
			if (ptr->next != NULL) {
				if (prev == NULL) {
					*dict = ptr->next;
				}
				else {
					prev->next = ptr->next;
				}
			}
			else if (prev != NULL) {
				prev->next = NULL;
			}
			else {
				*dict = NULL;
			}

			free(ptr->key);
			free(ptr);

			return;
		}
	}
}