#include "externDB.h"
#include "linkedList.h"

void free_extern_data(void * data);


/* database for words with extern attribute, uses LinkedListDB */
struct ExternDB
{
	LinkedListDB * linkedListDB;
};

/*iterator for words with extern attribute, uses LinkedListIterator*/
struct ExternIterator
{
	LinkedListIterator * iterator;
};


/* return an empty database ExternDB */
ExternDB * create_extern_db()
{
	ExternDB * externDB = check_memory_alloc(malloc(sizeof(ExternDB)));
	externDB->linkedListDB = create_linked_list_db();
	return externDB;
}


/* add word with extern attribute to ExternDB and return TRUE if success */
bool add_extern_to_db(ExternDB *externDB, ExternData * externData)
{
	return add_data_to_linked_list(externDB->linkedListDB, (void*)externData);
}


/* get ExternDB and return ExternIterator */
ExternIterator * init_extern_iteraror(ExternDB * externDB)
{
	ExternIterator * externIterator = check_memory_alloc(malloc(sizeof(ExternIterator)));
	externIterator->iterator = init_linked_list_iteraror(externDB->linkedListDB);
	return externIterator;
}


/* get next ExternData from ExternIterator */
ExternData * get_next_extern(ExternIterator * externIterator)
{
	return get_next_data(externIterator->iterator);
}


/* deallocate memory for ExternData */
void free_extern_data(void * data)
{
	ExternData * externData = (ExternData*)data;
	free((char *)externData->symbolName);
	free(externData);
}


/* deallocate memory for ExternDB */
void free_extern_db(ExternDB * externDB)
{
	free_linked_list_db(externDB->linkedListDB, free_extern_data);
	free(externDB);
}


/*deallocate memory for ExternIterator */
void free_extern_iteraror(ExternIterator * externIterator)
{
	free_linked_list_iteraror(externIterator->iterator);
	free(externIterator);
}
