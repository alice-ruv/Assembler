#include <stdlib.h>
#include "codeImageDB.h"
#include "linkedList.h"

void free_code_word(void * codeWord);


/*database for words with code attribute, uses LinkedListDB*/
struct CodeImageDB
{
	LinkedListDB * linkedListDB;
};

/*iterator for words with code attribute, uses LinkedListIterator*/
struct CodeImageIterator
{
	LinkedListIterator * iterator;
};


/* return an empty database CodeImageDB */
CodeImageDB * create_code_image_db()
{
	CodeImageDB * codeImageDB = check_memory_alloc(malloc(sizeof(CodeImageDB)));
	codeImageDB->linkedListDB = create_linked_list_db();
	return codeImageDB;
}


/* add word with code attribute to CodeImageDB and return TRUE if success*/
bool add_cmd_to_db(CodeImageDB * codeImageDB, Cmd * cmd)
{
	int i;

	for (i = 0; i < cmd->cmdInfo->numOfOperands + 1; i++)
	{
		if (!add_data_to_linked_list(codeImageDB->linkedListDB, (void *) cmd->cmdWords[i]))
		{
			return FALSE;
		}
	}

	return TRUE;
}


/* get CodeImageDB and return CodeImageIterator */
CodeImageIterator * init_code_image_iteraror(CodeImageDB * codeImageDB)
{
	CodeImageIterator * codeImageIterator = check_memory_alloc(malloc(sizeof(CodeImageIterator)));
	codeImageIterator->iterator = init_linked_list_iteraror(codeImageDB->linkedListDB);
	return codeImageIterator;
}


/* get next CodeWord from CodeImageIterator */
CodeWord * get_next_code_word(CodeImageIterator * codeImageIterator)
{
	return get_next_data(codeImageIterator->iterator);
}


/* deallocate memory for CodeWord */
void free_code_word(void * word)
{
	CodeWord * codeWord = (CodeWord*)word;
	free(codeWord);
}


/* deallocate memory for CodeImageDB */
void free_code_image_db(CodeImageDB * codeImageDB)
{
	free_linked_list_db(codeImageDB->linkedListDB, free_code_word);
	free(codeImageDB);
}


/* deallocate memory for CodeImageIterator */
void free_code_image_iteraror(CodeImageIterator * codeImageIterator)
{
	free_linked_list_iteraror(codeImageIterator->iterator);
	free(codeImageIterator);
}
