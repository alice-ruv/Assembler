#include "processHandler.h"

/* deallocate memory for ProcessHandler */
void free_memory(ProcessHandler * handler)
{
	free_extern_db(handler->externDB);
	handler->externDB = NULL;
	free_entry_db(handler->entryDB);
	handler->entryDB = NULL;
	free_symbol_db(handler->symbolDB);
	handler->symbolDB = NULL;
	free_code_image_db(handler->codeImageDB);
	handler->codeImageDB = NULL;
	free_data_image_db(handler->dataImageDB);
	handler->dataImageDB = NULL;
	free((char *)handler->inputFileName);
	handler->inputFileName = NULL;

}


/* initialize ProcessHandler for new line input */
void init_handler_for_line(ProcessHandler * handler)
{
	handler->labelName = NULL;
	handler->cmd = NULL;
	handler->commaHandleInput = NULL;
	handler->lineNumber++;
}


/* deallocate memory for ProcessHandler per line */
void free_handler_data_per_line(ProcessHandler * handler)
{
	if (handler->commaHandleInput != NULL)
	{
		free(handler->commaHandleInput);
		handler->commaHandleInput = NULL;
	}

	if (handler->cmd == NULL)
	{
		return;
	}

	/*no need to free the code words that in cmdWords*/
	/*if we are in p2, we cannot free the code words because they are stored in codeDB*/
	/*if we are in p1, we don't create code words*/

	if (handler->cmd->cmdWords != NULL)
	{
		free(handler->cmd->cmdWords);
	}

	free(handler->cmd);
	/*cmdInfo is stored in reservedwordDB, so we can't free it */
	handler->cmd = NULL;
}
