#include <string.h>
#include "fileHandler.h"


/* open file and return pointer to the file if success */
FILE * open_file(const char *fileName, const char * permissions)
{
	FILE * file = fopen(fileName, permissions);

	if (file == NULL) {
		printf("Error opening file %s\n", fileName);
		return NULL;
	}

	if (ferror(file)) {
		perror("Error opening file");
		return NULL;
	}

	return file;
}


/* close file and return TRUE if success */
bool close_file(FILE * file)
{
	if (fclose(file) != EOF) {
		return TRUE;
	}

	if (ferror(file)) {
		perror("Error closing file");
	}

	else {
		printf("Error closing file\n");
	}

	return FALSE;
}


/* rewind file and return TRUE if success */
bool rewind_file(FILE * file)
{

	if (fflush(file) == 0) {
		rewind(file);
		return TRUE;
	}

	if (ferror(file)) {
		perror("Error flushing file");
	}

	else {
		printf("Error flushing file\n");
	}

	return FALSE;
}


/* open file with file name handling */
FILE * open_file_v2(const char * fileNamePrefix, const char * fileNameSuffix, const char * permissions)
{
	FILE * outputFile = NULL;
	char * extFileName = check_memory_alloc(calloc(strlen(fileNamePrefix) + strlen(fileNameSuffix) + 1, sizeof(char)));
	strcpy(extFileName, fileNamePrefix);
	outputFile = open_file(strcat(extFileName, fileNameSuffix), permissions);
	free(extFileName);
	if (outputFile == NULL)
	{
		printf("failed to open %s%s file.\n", fileNamePrefix, fileNameSuffix);
	}
	return  outputFile;
}


/* check validation of file path and return TRUE in case file path is valid */
bool is_file_path_valid(const char * filePath)
{
	int pathSize = strlen(filePath);
	if (pathSize > FILENAME_MAX)
	{
		printf("file path is %d. the maximum is %d \n", pathSize, FILENAME_MAX);
		return FALSE;
	}

	if (filePath[pathSize - 1] == '/')
	{
		printf("file path cannot end with '/' char.\n");
		return FALSE;
	}

	return TRUE;
}
