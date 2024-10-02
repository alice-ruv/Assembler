#include "utils.h"
#include "assemblerDefinitions.h"

#ifndef SYMBOL_DB_INCLUDED
#define SYMBOL_DB_INCLUDED


typedef struct SymbolDB SymbolDB;

bool add_symbol(SymbolDB * symblolDB, const char * symbolName, int value, attributeFlag attribute);
SymbolDB * create_symbol_db();
SymbolData * try_get_symbol_data(SymbolDB* symblolDB, const char * symbolName);
void free_symbol_db(SymbolDB * symbolDB);

#endif
