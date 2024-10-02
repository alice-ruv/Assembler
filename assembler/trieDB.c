#include "trieDB.h"
#include <string.h>
#include <stdlib.h>

#define NUM_ALPHABET 26
#define NUM_DIGITS 10
#define ARR_SIZE (NUM_ALPHABET *2 + NUM_DIGITS)

/*node of TrieDB*/
typedef struct Node
{
	void * data;
	struct Node ** arr;  /*array of pointers to node in TrieDB*/
} Node;

/* TrieDB represents a specific tree data structure.
 * Each node represents a specific character in field "arr" of TrieDB which may point to another character in the next node.
 * Supports only keys with digits and upper/lower case alphabet letters.
 * Time and space complexity of adding new data and retrieving existing data is O(L), where L is length of the key.
 * The node which contains the last character will contain the appropriate value in its data field.*/
struct TrieDB
{
   Node* firstNode;
};

Node * create_trie_node();
bool add_data_to_trie_internal(Node* node, const char * key, void * data, int index, int keySize);
void * get_data_from_trie_internal(Node* firstNode, const char * key, int index, int keySize);
int get_arr_index(const char c);
void free_trie_db_internal(Node * node, trieFreeData freeData);


/* create new database TrieDB */
TrieDB * create_trie_db()
{
	TrieDB * trieDB = check_memory_alloc(malloc(sizeof(TrieDB)));
    trieDB->firstNode = create_trie_node();
    return trieDB;
}


/* create new node of TrieDB */
Node * create_trie_node()
{
    Node * newNode = check_memory_alloc(malloc(sizeof(Node)));
    newNode->data = NULL;
 	newNode->arr = NULL;
    return newNode;
}

/* get input character and return integer that represents the node* array index*/
int get_arr_index(const char c)
{
	if ((c >= '0') && (c <= '9'))
	{
		return c - '0';
	}

	if ((c >= 'A') && (c <= 'Z'))
	{
		return c - 'A' + NUM_DIGITS;
	}

	if ((c >= 'a') && (c <= 'z'))
	{
		return c - 'a' + NUM_DIGITS + NUM_ALPHABET;
	}

	/*TrieDB does not support non-alphanumeric characters*/
	return -1;
}


/* add data to TrieDB and return TRUE if success */
bool add_data_to_trie(TrieDB * db, const char * key, void * data)
{
    int keySize = strlen(key);
	return add_data_to_trie_internal(db->firstNode, key,  data, 0, keySize);
}


/* add data (in the location that the key represent) to TrieDB recursively and return TRUE if success
 * the depth of the recursive is as the length of the key
 * in each iteration we advance to next node on the array that represents the current character we are working on */
bool add_data_to_trie_internal(Node* node, const char * key, void * data, int index, int keySize)
{
    int arrIndex;

    if (index == keySize)
    {
       if (node->data != NULL)
       {
       		return FALSE;  /*return FALSE in case data is already exists on this key*/
       }
       
       node->data = data;
       return TRUE;
    }
    
    arrIndex = get_arr_index(key[index]);

    if (arrIndex == -1) {
    	return FALSE;
    }

    if (node->arr == NULL) {
    	node->arr = (Node**)check_memory_alloc(calloc(ARR_SIZE, sizeof(Node*)));
    }
    
    if (node->arr[arrIndex] == NULL) {
    	node->arr[arrIndex] = create_trie_node();
    }

    return add_data_to_trie_internal(node->arr[arrIndex], key, data, ++index, keySize);
}


/* get data from TrieDB on the key and return TRUE if success */
void * get_data_from_trie(TrieDB * db, const char * key)
{
  int keySize = strlen(key);
  return  get_data_from_trie_internal(db->firstNode, key, 0, keySize);
}


/* get data from TrieDB on the key and return TRUE if success
 * this is recursive function. the depth of the recursive is as the length of the key.
 * on each iteration we advance to next node on the array that represent the current character we are working on*/
void * get_data_from_trie_internal(Node* node, const char * key, int index, int keySize)
{
	int arrIndex;

    if (index == keySize) {
       return node->data;  
    }

    arrIndex = get_arr_index(key[index]);

    if (arrIndex == -1) {
		return FALSE;
    }
    
    return ((node->arr == NULL) || (node->arr[arrIndex] == NULL)) ? NULL : get_data_from_trie_internal(node->arr[arrIndex], key, ++index, keySize);
}


/* deallocate memory for TrieDB */
void free_trie_db(TrieDB * db, trieFreeData freeData)
{
	free_trie_db_internal(db->firstNode, freeData);
	free(db);
}


/* deallocate memory for TrieDB
* this is recursive function. the depth of the recursive is as the max(length(keys))*/
void free_trie_db_internal(Node * node, trieFreeData freeData)
{
    if (node->data != NULL) {
    	freeData(node->data);
    }

	if (node->arr != NULL)
	{
		int i;
		for(i = 0; i < ARR_SIZE; i++)
		{
			if (node->arr[i] != NULL)
			{
				free_trie_db_internal(node->arr[i], freeData);
				node->arr[i] = NULL;
			}
		}
		free(node->arr);
		node->arr = NULL;
	}

	free(node);
}
