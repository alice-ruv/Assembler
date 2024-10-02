#include "utils.h"
#include "assemblerDefinitions.h"

#ifndef CODE_IMAGE_INCLUDED
#define CODE_IMAGE_INCLUDED

typedef struct CodeImageDB CodeImageDB;
typedef struct CodeImageIterator CodeImageIterator;

CodeImageDB * create_code_image_db();
bool add_cmd_to_db(CodeImageDB * codeImageDB, Cmd * cmd);
CodeImageIterator * init_code_image_iteraror(CodeImageDB * codeImageDB);
CodeWord * get_next_code_word(CodeImageIterator * codeImageIterator);

void free_code_image_db(CodeImageDB * codeImageDB);
void free_code_image_iteraror(CodeImageIterator * codeImageIterator);

#endif
