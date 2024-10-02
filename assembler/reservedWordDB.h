#include "utils.h"
#include "assemblerDefinitions.h"

#ifndef RESERVED_WORD_DB_INCLUDED
#define RESERVED_WORD_DB_INCLUDED

typedef struct ReservedWordDB ReservedWordDB;

typedef enum{TYPE_REG, TYPE_CMD, TYPE_DATA, TYPE_STRING, TYPE_ENTRY, TYPE_EXTERN, TYPE_NOT_RESERVED} ReservedWordType;

ReservedWordDB * init_reserved_word_db();
bool is_reserved_word(ReservedWordDB* db, const char * input);
ReservedWordType get_reserved_word_type(ReservedWordDB* db, const char * input);
CmdInfo * get_cmd_info(ReservedWordDB* db, const char * input);
Reg * get_reg_info(ReservedWordDB* db, const char * input);
void free_reserved_word_db(ReservedWordDB * reservedWordDB);

#endif
