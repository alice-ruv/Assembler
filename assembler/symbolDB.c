#include <stdlib.h>
#include "symbolDB.h"
#include "trieDB.h"

/*database for symbol table, uses TrieDB*/
struct SymbolDB
{
	TrieDB * dataBase;
};

void free_symbol_data(void * data);


/* create new SymbolDB database */
SymbolDB * create_symbol_db()
{
   SymbolDB * symblolDB = check_memory_alloc(malloc(sizeof(SymbolDB)));
   symblolDB->dataBase = create_trie_db();

   return symblolDB;
}

/* add symbol to SymbolDB and return TRUE if success */
bool add_symbol(SymbolDB * symblolDB, const char * symbolName, int value, attributeFlag attribute)
{
	SymbolData * symbolData = check_memory_alloc(malloc(sizeof(SymbolData)));

	symbolData->value = value;
	symbolData->attributeFlags = attribute;

	if (add_data_to_trie(symblolDB->dataBase, symbolName, (void *)symbolData) == FALSE)
	{
		free(symbolData);
		return FALSE;
	}

	return TRUE;
}


/* get symbol name and SymbolDB database and return SymbolData*/
SymbolData * try_get_symbol_data(SymbolDB * symblolDB, const char * symbolName)
{
	return (SymbolData *)get_data_from_trie(symblolDB->dataBase, symbolName);
}


/* deallocate memory for SymbolData */
void free_symbol_data(void * data)
{
	SymbolData * symbolData = (SymbolData*)data;
	free(symbolData);
}


/* deallocate memory for SymbolDataDB */
void free_symbol_db(SymbolDB * symbolDB)
{
	free_trie_db(symbolDB->dataBase, free_symbol_data);
	free(symbolDB);
}

