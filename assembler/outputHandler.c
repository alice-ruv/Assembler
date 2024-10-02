#include <stdio.h>
#include <string.h>
#include "outputHandler.h"
#include "fileHandler.h"

FILE * create_output_file(const char * fileNamePrefix, const char * fileNameSuffix);
void print_memory_image(ProcessHandler * handler);
void print_symbols(ProcessHandler * handler);


/* get ProcessHandler and print output*/
void print_output(ProcessHandler * handler)
{
	print_memory_image(handler);
	print_symbols(handler);
}


/* get output file name prefix and suffix, return pointer to output file */
FILE * create_output_file(const char * fileNamePrefix, const char * fileNameSuffix)
{
	return open_file_v2(fileNamePrefix, fileNameSuffix, "w+");
}


/* get ProcessHandler and print memory image */
void print_memory_image(ProcessHandler * handler)
{
	int codeImageSize = (handler->IC) - FIRST_ADDRESS;
	int dataImageSize = handler->DC;
	int address = FIRST_ADDRESS;
	CodeWord * codeWord = NULL;
	DataWord * dataWord = NULL;
	CodeImageIterator * codeImageIterator;
	DataImageIterator * dataImageIterator;

	FILE * out_obFile = create_output_file(handler->fileNamePrefix, ".ob");
	if (out_obFile == NULL)
	{
		return ;
	}

	codeImageIterator = init_code_image_iteraror(handler->codeImageDB);
	dataImageIterator = init_data_image_iterator(handler->dataImageDB);

	fprintf(out_obFile, "    %d  %0d\n" , codeImageSize, dataImageSize);

	/*print code image in file ended with .ob*/
	while((codeWord = get_next_code_word(codeImageIterator)) != NULL){
		fprintf(out_obFile, " %04d  %03X  %c\n" ,address++, codeWord->codeRep, codeWord->type);
	}

	/* data image will be printed in the same file only after code image*/
	while((dataWord = get_next_data_word(dataImageIterator)) != NULL){
		fprintf(out_obFile, " %04d  %03X  %c\n" ,address++, dataWord->dataRep, dataWord->type);
	}

	free_code_image_iteraror(codeImageIterator);
	free_data_image_iteraror(dataImageIterator);

	close_file(out_obFile);
}


/* get ProcessHandler and print output files for extern and entry labels */
void print_symbols(ProcessHandler * handler)
{
	ExternData * externData = NULL;
	EntryData * entryData = NULL;

	FILE * out_extFile = NULL;
	FILE * out_entFile = NULL;

	ExternIterator * externIterator = init_extern_iteraror(handler->externDB);
	EntryIterator * entryIterator = init_entry_iteraror(handler->entryDB);

	/*print extern labels if exist in file ended with .ext*/
	while((externData = get_next_extern(externIterator)) != NULL)
	{
		if (out_extFile == NULL)
		{
			out_extFile = create_output_file(handler->fileNamePrefix, ".ext");
			if (out_extFile == FALSE)
			{
				break;
			}
		}

		fprintf(out_extFile," %s  %04d\n", externData->symbolName, externData->address);
	}

	/*print entry labels if exist in file ended with .ent*/
	while((entryData = get_next_entry(entryIterator)) != NULL)
	{
		if (out_entFile == NULL)
		{
			out_entFile = create_output_file(handler->fileNamePrefix, ".ent");
			if (out_entFile == FALSE)
			{
				break;
			}
		}

		fprintf(out_entFile," %s  %04d\n", entryData->symbolName, entryData->address);
	}

	free_extern_iteraror(externIterator);
	free_entry_iteraror(entryIterator);

	if (out_entFile != NULL)
	{
		close_file(out_entFile);
	}

	if (out_extFile != NULL)
	{
		close_file(out_extFile);
	}
} /*end of print_symbols*/
