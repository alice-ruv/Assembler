#include <string.h>
#include "assemblerHandler.h"
#include "processHandler.h"
#include "fileHandler.h"
#include "firstPass.h"
#include "secondPass.h"
#include "outputHandler.h"


bool process_file(const char * fileNamePrefix, ReservedWordDB * reservedWordDB, PassFunctions * passFunctions);


/* return TRUE in case input file has no errors */
bool process_file(const char * fileNamePrefix, ReservedWordDB * reservedWordDB, PassFunctions * passFunctions)
{
	char * inputFileName;
	FILE * file;
	ProcessHandler processHandler;

    if (is_file_path_valid(fileNamePrefix) == FALSE)
    {
    	return FALSE;
    }

	inputFileName = check_memory_alloc(calloc(strlen(fileNamePrefix) + strlen(".as") + 1, sizeof(char)));


	strcpy(inputFileName, fileNamePrefix);
	file = open_file(strcat(inputFileName, ".as"), "r");

	if (file == NULL)
	{
		free(inputFileName);
		return FALSE;
	}

	processHandler.inputFileName = inputFileName;
	processHandler.fileNamePrefix = fileNamePrefix;
	processHandler.reservedWordDB = reservedWordDB;

	processHandler.symbolDB = create_symbol_db();
	processHandler.codeImageDB = create_code_image_db();
	processHandler.dataImageDB = create_data_image_db();
	processHandler.externDB = create_extern_db();
	processHandler.entryDB = create_entry_db();

	processHandler.passFunctions = passFunctions;
	processHandler.pass = P1;
	processHandler.IC = FIRST_ADDRESS;
	processHandler.DC = 0;

	if (execute_first_pass(file, &processHandler) == FALSE)
	{
		printf("Error in first pass file %s: will not execute second pass.\n", processHandler.inputFileName);
		close_file(file);
		free_memory(&processHandler);
		return FALSE;
	}

	processHandler.lineNumber = 0;
	processHandler.pass = P2;
	processHandler.ICF = processHandler.IC;
	processHandler.IC = 100;

	if (rewind_file(file) == FALSE)
	{
		return FALSE;
	}

	if (execute_second_pass(file, &processHandler) == FALSE)
	{
		printf("Error in second pass file %s: will not create output files.\n", processHandler.inputFileName);
		close_file(file);
		free_memory(&processHandler);
		return FALSE;
	}

	print_output(&processHandler);

	free_memory(&processHandler);

	return close_file(file);

} /*end of process_file*/


/* return TRUE in case input files has no errors */
bool process_files(char ** fileNamePrefixArr, int numOfFiles)
{
	ReservedWordDB * reservedWordDB;
	PassFunctions passFunctions;
	int i;
	bool res = TRUE;
	parse_input parseInput[NUM_PASSES] = {0};
	end_cmd finishCmd[NUM_PASSES] = {0};
	passFunctions.parseInput = parseInput;
	passFunctions.endCmd = finishCmd;

	reservedWordDB = init_reserved_word_db();

	for(i = 0; i < numOfFiles; i++)
	{
		if(process_file(fileNamePrefixArr[i], reservedWordDB, &passFunctions) == FALSE && res == TRUE)
		{
			res = FALSE;
		}
	}

	free_reserved_word_db(reservedWordDB);
	return res;
}
