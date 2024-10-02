#include "utils.h"
#include "assemblerDefinitions.h"

#ifndef EXTERN_DB_INCLUDED
#define EXTERN_DB_INCLUDED

typedef struct ExternDB ExternDB;
typedef struct ExternIterator ExternIterator;

ExternDB * create_extern_db();
bool add_extern_to_db(ExternDB *externDB, ExternData * externData);
ExternIterator * init_extern_iteraror(ExternDB * externDB);
ExternData * get_next_extern(ExternIterator * externIterator);
void free_extern_db(ExternDB * externDB);
void free_extern_iteraror(ExternIterator * externIterator);

#endif



