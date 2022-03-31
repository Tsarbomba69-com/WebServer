#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Nodes are used to store data of any type in a list.  
typedef struct Node {
	/* PUBLIC MEMBER VARIABLES */
	// The data is stored as a void pointer - casting is required for proper access.
	void* data;
	// A pointer to the next node in the chain.
	Node* next;
	Node* previous;
} Node;

typedef struct LinkedList {
	Node* head;
	// Length refers to the number of nodes in the chain.
	int length;
} LinkedList;


Node node_constructor(void* data, unsigned long size);

void node_destructor(struct Node* node);

LinkedList linked_list_constructor();

void insert(LinkedList* linked_list, int index, void* data, unsigned long size);

Node* create_node(void* data, unsigned long size);

Node* iterate(LinkedList* linked_list, int index);

void remove_node(LinkedList* linked_list, int index);

LinkedList linked_list_constructor(void);

void linked_list_destructor(LinkedList* linked_list);

void* get_list_item(LinkedList* linked_list, int index);