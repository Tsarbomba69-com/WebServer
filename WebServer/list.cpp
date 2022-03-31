#include "list.h"


// The constructor is used to create new instances of nodes.
Node node_constructor(void* data, unsigned long size)
{
	if (size < 1)
	{
		// Confirm the size of the data is at least one, otherwise exit with an error message.
		printf("Invalid data size for node...\n");
		exit(1);
	}
	// Create a Node instance to be returned
	struct Node node;
	// Allocate space for the data if it is of a supported type
	node.data = data;
	//memset(node.data, 0, size);
	//memcpy(node.data, data, size);
	// Initialize the pointers.
	node.next = NULL;
	node.previous = NULL;
	return node;
}

// The destructor removes a node by freeing the node's data and its node.
void node_destructor(struct Node* node)
{
	free(node->data);
	free(node);
}

LinkedList linked_list_constructor()
{
	struct LinkedList new_list;
	new_list.head = NULL;
	new_list.length = 0;
	return new_list;
}

// The insert function puts a new node in the chain.
void insert(LinkedList* linked_list, int index, void* data, unsigned long size) {
	// Create a new node to be inserted.
	struct Node* node_to_insert = create_node(data, size);
	// Check if this node will be the new head of the list.
	if (index == 0)
	{
		// Define the Node's next.
		node_to_insert->next = linked_list->head;
		// Re-define the List's head.
		linked_list->head = node_to_insert;
	}
	else
	{
		// Find the item in the list immediately before the desired index.
		Node* cursor = iterate(linked_list, index - 1);
		// Set the Node's next.
		node_to_insert->next = cursor->next;
		// Set the cursor's next to the new node.
		cursor->next = node_to_insert;

	}
	// Increment the list length.
	linked_list->length += 1;
}

void linked_list_destructor(LinkedList* linked_list)
{
	for (int i = 0; i < linked_list->length; i++)
	{
		remove_node(linked_list, 0);
	}
}

// The remove function removes a node from the linked list.
void remove_node(LinkedList* linked_list, int index)
{
	// Check if the item being removed is the head.
	if (index == 0)
	{
		// Collect the node to be removed.
		Node* node_to_remove = linked_list->head;
		// Define the new head of the list.
		if (node_to_remove)
		{
			linked_list->head = node_to_remove->next;
			// Remove the desired node.
			node_destructor(node_to_remove);
		}
	}
	else
	{
		// Find the item in the list before the one that is going to be removed.
		struct Node* cursor = iterate(linked_list, index - 1);
		// Use the cursor to define the node to be removed.
		struct Node* node_to_remove = cursor->next;
		// Update the cursor's next to skip the node to be removed.
		cursor->next = node_to_remove->next;
		// Remove the node.
		node_destructor(node_to_remove);
	}
	// Decrement the list length.
	linked_list->length -= 1;
}

Node* create_node(void* data, unsigned long size)
{
	// Allocate space.
	Node* new_node = (Node*)malloc(sizeof(Node));
	// Call the constructor.
	*new_node = node_constructor(data, size);
	return new_node;
}



// The iterate function traverses the list from beginning to end.
Node* iterate(LinkedList* linked_list, int index)
{
	// Confirm the user has specified a valid index.
	if (index < 0 || index >= linked_list->length)
	{
		return NULL;
	}
	// Create a cursor node for iteration.
	struct Node* cursor = linked_list->head;
	// Step through the list until the desired index is reached.
	for (int i = 0; i < index; i++)
	{
		cursor = cursor->next;
	}
	return cursor;
}

void* get_list_item(LinkedList* linked_list, int index) {
	return iterate(linked_list, index)->data;
}