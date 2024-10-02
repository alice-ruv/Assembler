#include <stdlib.h>
#include "linkedList.h"

/*node of linked list database*/
typedef struct Node
{
  void * data; 
  struct Node * next;
} Node;

/*linked list database*/
/*complexity of adding new data is O(1)*/
struct LinkedListDB
{
	Node* head;
	Node* current;
};

/*iterator of linked list database*/
/* complexity of getting next data is O(1)*/
struct LinkedListIterator
{
	Node * current;
};


Node * create_linked_list_node(void * data);


/* create new database LinkedListDB */
LinkedListDB * create_linked_list_db()
{
	LinkedListDB * linkedListDB = check_memory_alloc(malloc(sizeof(LinkedListDB)));
	linkedListDB->current = NULL;
	linkedListDB->head = NULL;
	return linkedListDB;
}


/* get data to create a new node of LinkedListDB and return pointer it */
Node * create_linked_list_node(void * data)
{
	Node * newNode = check_memory_alloc(malloc(sizeof(Node)));
	newNode->data = data;
	newNode->next = NULL;
	return newNode;
}


/* add data to LinkedListDB and return TRUE if success */
bool add_data_to_linked_list(LinkedListDB * linkedListDB, void * data)
{
	Node * newNode = create_linked_list_node(data);

	if (linkedListDB->head == NULL)
	{
		linkedListDB->head = newNode;
		linkedListDB->current = newNode;
		return TRUE;
	}

	linkedListDB->current->next = newNode;
	linkedListDB->current = newNode;
	return TRUE;
}


/* get LinkedListDB and return LinkedListIterator*/
LinkedListIterator * init_linked_list_iteraror(LinkedListDB * linkedListDB)
{
	LinkedListIterator * linkedListIterator = check_memory_alloc(malloc(sizeof(LinkedListIterator)));
	linkedListIterator->current = linkedListDB->head;
	return linkedListIterator;
}


/* get next data from LinkedListIterator */
void * get_next_data(LinkedListIterator * iterator)
{
	Node * currNode = iterator->current;
	if(currNode == NULL)
	{
		return NULL;
	}

	iterator->current = iterator->current->next;
	return currNode->data;
}


/* deallocate memory for LinkedListDB */
void free_linked_list_db(LinkedListDB * linkedListDB, linkedListfreeData freeData)
{
	Node * node = linkedListDB->head;

	while(node != NULL)
	{
		Node * nodeToFree = node;
		node = node->next;
		freeData(nodeToFree->data);
		free(nodeToFree);
	}

	linkedListDB->head = NULL;
	linkedListDB->current = NULL;
	free(linkedListDB);
}


/* deallocate memory for LinkedListIterator */
void free_linked_list_iteraror(LinkedListIterator * linkedListIterator)
{
	free(linkedListIterator);
}

