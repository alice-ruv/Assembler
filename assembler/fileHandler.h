#include <stdio.h>
#include "utils.h"

bool close_file(FILE * file);
bool rewind_file(FILE * file);
FILE * open_file(const char *fileName, const char * permissions);
FILE * open_file_v2(const char * fileNamePrefix, const char * fileNameSuffix, const char * permissions);
bool is_file_path_valid(const char * filePath);
