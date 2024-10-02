#include "utils.h"
#include "assemblerDefinitions.h"

#ifndef ENTRY_DB_INCLUDED
#define ENTRY_DB_INCLUDED

typedef struct EntryDB EntryDB;
typedef struct EntryIterator EntryIterator;

EntryDB * create_entry_db();
bool add_entry_to_db(EntryDB *entryDB, EntryData * entryData);
EntryIterator * init_entry_iteraror(EntryDB * entryDB);
EntryData * get_next_entry(EntryIterator * entryIterator);
void free_entry_db(EntryDB *entryDB);
void free_entry_iteraror(EntryIterator * entryIterator);

#endif
