#include <stdlib.h>
#include <string.h>
#include "dataImageDB.h"
#include "linkedList.h"

DataWord * create_data_word(unsigned int data);
void free_data_word(void * word);


/*database for words with data attribute (stores strings and integer values), uses LinkedListDB*/
struct DataImageDB
{
	LinkedListDB * linkedListDB;
};

/*iterator for words with data attribute, uses LinkedListIterator*/
struct DataImageIterator
{
	LinkedListIterator * iterator;
};


/* return an empty database DataImageDB */
DataImageDB * create_data_image_db()
{
	DataImageDB * dataImageDB = check_memory_alloc(malloc(sizeof(DataImageDB)));
	dataImageDB->linkedListDB = create_linked_list_db();
	return dataImageDB;
}


/* add string to DataImageDB and return TRUE in success */
bool add_string_to_db(DataImageDB * dataImageDB, const char * string, int size )
{
	int i;
	for(i = 0; i < size; i++)
	{
		if (add_data_to_linked_list(dataImageDB->linkedListDB, (void*)create_data_word(string[i])) == FALSE)
		{
			return FALSE;
		}
	}

	/* adding '/0' to mark end of string*/
	return add_data_to_linked_list(dataImageDB->linkedListDB,(void*)create_data_word('\0'));
}


/* add integer value to DataImageDB and return TRUE in success */
bool add_int_to_db(DataImageDB * dataImageDB, int number)
{
	return add_data_to_linked_list(dataImageDB->linkedListDB,(void*)create_data_word(number));
}


/* return word with data attribute */
DataWord * create_data_word(unsigned int data)
{
	DataWord * dataWord = check_memory_alloc(malloc(sizeof(DataWord)));
	dataWord->dataRep = data;
	dataWord->type = TYPE_A;
	return dataWord;
}


/* get DataImageDB and return DataImageIterator */
DataImageIterator * init_data_image_iterator(DataImageDB * dataImageDB)
{
	DataImageIterator * dataImageIterator = check_memory_alloc(malloc(sizeof(DataImageIterator)));
	dataImageIterator->iterator = init_linked_list_iteraror(dataImageDB->linkedListDB);
	return dataImageIterator;
}


/* get next DataWord from DataImageIterator */
DataWord * get_next_data_word(DataImageIterator * dataImageIterator)
{
	return get_next_data(dataImageIterator->iterator);
}


/* deallocate memory for DataWord */
void free_data_word(void * word)
{
	DataWord * dataWord = (DataWord*)word;
	free(dataWord);
}


/* deallocate memory for DataImageDB */
void free_data_image_db(DataImageDB * dataImageDB)
{
	free_linked_list_db(dataImageDB->linkedListDB, free_data_word);
	free(dataImageDB);
}


/* deallocate memory for DataImageIterator */
void free_data_image_iteraror(DataImageIterator * dataImageIterator)
{
	free_linked_list_iteraror(dataImageIterator->iterator);
	free(dataImageIterator);
}
