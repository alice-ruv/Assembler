#include "entryDB.h"
#include "linkedList.h"

void free_entry_data(void * data);


/*database for words with entry attribute, uses LinkedListDB*/
struct EntryDB
{
	LinkedListDB * linkedListDB;
};

/*iterator for words with entry attribute, uses LinkedListIterator*/
struct EntryIterator
{
	LinkedListIterator * iterator;
};


/* return an empty database EntryDB */
EntryDB * create_entry_db()
{
	EntryDB * entryDB = check_memory_alloc(malloc(sizeof(EntryDB)));
	entryDB->linkedListDB = create_linked_list_db();
	return entryDB;
}


/* add word with entry attribute to EntryDB and return TRUE if success */
bool add_entry_to_db(EntryDB * entryDB, EntryData * entryData)
{
	return add_data_to_linked_list(entryDB->linkedListDB, (void*)entryData);
}


/* get EntryDB and return EntryIterator */
EntryIterator * init_entry_iteraror(EntryDB * entryDB)
{
	EntryIterator * entryIterator = check_memory_alloc(malloc(sizeof(EntryIterator)));
	entryIterator->iterator = init_linked_list_iteraror(entryDB->linkedListDB);
	return entryIterator;
}


/* get next EntryData from EntryIterator */
EntryData * get_next_entry(EntryIterator * entryIterator)
{
	return get_next_data(entryIterator->iterator);
}


/* deallocate memory for EntryData */
void free_entry_data(void * data)
{
	EntryData * entryData = (EntryData*)data;
	free((char *)entryData->symbolName);
	free(entryData);
}


/* deallocate memory for EntryDB */
void free_entry_db(EntryDB *entryDB)
{
	free_linked_list_db(entryDB->linkedListDB, free_entry_data);
	free(entryDB);
}


/* deallocate memory for EntryIterator */
void free_entry_iteraror(EntryIterator * entryIterator)
{
	free_linked_list_iteraror(entryIterator->iterator);
	free(entryIterator);
}
