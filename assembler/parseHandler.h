#include "processHandler.h"

bool parse_file(FILE * inputFile, ProcessHandler * handler);
state handle_cmd_name(const char * input, ProcessHandler * handler);
bool parse_number(const char * valInput, int * out_num);



