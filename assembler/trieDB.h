#include "utils.h"

#ifndef DATA_IMAGE_INCLUDED
#define DATA_IMAGE_INCLUDED

struct TrieDB;
typedef struct TrieDB TrieDB;

/*pointer to function for releasing stored data in TrieDB*/
typedef  void (* trieFreeData)(void *);

TrieDB * create_trie_db();
bool add_data_to_trie(TrieDB* db, const char * key, void * data);
void * get_data_from_trie(TrieDB * db, const char * key);
void  free_trie_db(TrieDB * db, trieFreeData freeData);

#endif

