#include <stdlib.h>
#include "assemblerDefinitions.h"
#include "reservedWordDB.h"
#include "trieDB.h"

/*database for reserved words, uses TrieDB*/
struct ReservedWordDB
{
	TrieDB * trieDB;
};

/*reserved words which refer to code*/
typedef union
{
 Reg* reg;
 CmdInfo * cmdInfo;
} CodeReservedWord;

typedef struct
{
ReservedWordType type;
CodeReservedWord codeReservedWord;
}ReservedWordData;

bool add_data_to_db(ReservedWordDB* db, const char * dataName, ReservedWordType type);
bool add_cmd_info_to_db(ReservedWordDB* db, const char * cmdName,unsigned int optcode, unsigned int funct,
		int legalOriginAddressMode, int legalDestAddressMode, NumOfOperands numOfOperands);
void free_reserved_word_data(void * data);


/* create new ReservedWordDB database */
ReservedWordDB * init_reserved_word_db()
{
	/*add information of all the registers to reservedWordDB*/
	int i;
	char regName[2] = {'r'}; 
	ReservedWordDB * reservedWordDB = check_memory_alloc(malloc(sizeof(ReservedWordDB)));
    reservedWordDB->trieDB = create_trie_db();
	
	for(i = 0 ; i < NUM_OF_REGISTERS ; i++)
	{
		ReservedWordData * reservedWordData = check_memory_alloc(malloc(sizeof(ReservedWordData)));

		reservedWordData->type = TYPE_REG;
		reservedWordData->codeReservedWord.reg = check_memory_alloc(malloc(sizeof(Reg)));
		reservedWordData->codeReservedWord.reg->regRep = (1 << i); /*bit in location i will be set to 1 according to the register number*/
		
		regName[1] = '0' + i; /*r concatenated to char '0'-'7'*/
		add_data_to_trie(reservedWordDB->trieDB, regName, reservedWordData);
	}
	
	/*add information of all the instructions to reservedWordDB*/
	add_cmd_info_to_db(reservedWordDB, "mov", 0, 0, IMMEDIATE_FLAG | DIRECT_FLAG | REGISTER_FLAG, DIRECT_FLAG | REGISTER_FLAG, TWO_OPERANDS);
	add_cmd_info_to_db(reservedWordDB, "cmp", 1, 0, IMMEDIATE_FLAG | DIRECT_FLAG | REGISTER_FLAG, IMMEDIATE_FLAG | DIRECT_FLAG | REGISTER_FLAG, TWO_OPERANDS);
	add_cmd_info_to_db(reservedWordDB, "add", 2, 10, IMMEDIATE_FLAG | DIRECT_FLAG | REGISTER_FLAG, DIRECT_FLAG | REGISTER_FLAG, TWO_OPERANDS);
	add_cmd_info_to_db(reservedWordDB, "sub", 2, 11, IMMEDIATE_FLAG | DIRECT_FLAG | REGISTER_FLAG, DIRECT_FLAG | REGISTER_FLAG, TWO_OPERANDS);
	add_cmd_info_to_db(reservedWordDB, "lea", 4, 0, DIRECT_FLAG, DIRECT_FLAG | REGISTER_FLAG, TWO_OPERANDS);
	add_cmd_info_to_db(reservedWordDB, "clr", 5, 10, NO_OPERAND_FLAG, DIRECT_FLAG | REGISTER_FLAG, ONE_OPERAND);
	add_cmd_info_to_db(reservedWordDB, "not", 5, 11, NO_OPERAND_FLAG, DIRECT_FLAG | REGISTER_FLAG, ONE_OPERAND);
	add_cmd_info_to_db(reservedWordDB, "inc", 5, 12, NO_OPERAND_FLAG, DIRECT_FLAG | REGISTER_FLAG, ONE_OPERAND);
	add_cmd_info_to_db(reservedWordDB, "dec", 5, 13, NO_OPERAND_FLAG, DIRECT_FLAG | REGISTER_FLAG, ONE_OPERAND);
	add_cmd_info_to_db(reservedWordDB, "jmp", 9, 10, NO_OPERAND_FLAG, DIRECT_FLAG | RELATIVE_FLAG, ONE_OPERAND);
	add_cmd_info_to_db(reservedWordDB, "bne", 9, 11, NO_OPERAND_FLAG, DIRECT_FLAG | RELATIVE_FLAG, ONE_OPERAND);
	add_cmd_info_to_db(reservedWordDB, "jsr", 9, 12, NO_OPERAND_FLAG, DIRECT_FLAG | RELATIVE_FLAG, ONE_OPERAND);
	add_cmd_info_to_db(reservedWordDB, "red", 12, 0, NO_OPERAND_FLAG, DIRECT_FLAG | REGISTER_FLAG, ONE_OPERAND);
	add_cmd_info_to_db(reservedWordDB, "prn", 13, 0, NO_OPERAND_FLAG, IMMEDIATE_FLAG | DIRECT_FLAG | REGISTER_FLAG, ONE_OPERAND);
	add_cmd_info_to_db(reservedWordDB, "rts", 14, 0, NO_OPERAND_FLAG, NO_OPERAND_FLAG, NO_OPERANDS);
	add_cmd_info_to_db(reservedWordDB, "stop", 15, 0, NO_OPERAND_FLAG, NO_OPERAND_FLAG, NO_OPERANDS);
	
	/*add information of all the directives to reservedWordDB*/
	add_data_to_db(reservedWordDB, "data", TYPE_DATA);
	add_data_to_db(reservedWordDB, "string", TYPE_STRING);
	add_data_to_db(reservedWordDB, "entry", TYPE_ENTRY);
	add_data_to_db(reservedWordDB, "extern", TYPE_EXTERN);

	return reservedWordDB;
} /*end of init_reserved_word_db*/


/* add directives information to ReservedWordDB and return true if success */
bool add_data_to_db(ReservedWordDB* db, const char * dataName, ReservedWordType type)
{
	ReservedWordData * reservedWordData = check_memory_alloc(malloc(sizeof(ReservedWordData)));
	reservedWordData->type = type;
	return add_data_to_trie(db->trieDB, dataName, reservedWordData);
}


/* add instruction information to ReservedWordDB and return true if success */
bool add_cmd_info_to_db(ReservedWordDB* db, const char * cmdName, unsigned int optcode, unsigned int funct,
	 int legalOriginAddressMode, int legalDestAddressMode, NumOfOperands numOfOperands)
{
	ReservedWordData * reservedWordData = check_memory_alloc(malloc(sizeof(ReservedWordData)));
	reservedWordData->type = TYPE_CMD;
	reservedWordData->codeReservedWord.cmdInfo = check_memory_alloc(malloc(sizeof(CmdInfo)));
	reservedWordData->codeReservedWord.cmdInfo->optcode = optcode;
	reservedWordData->codeReservedWord.cmdInfo->funct = funct;
	reservedWordData->codeReservedWord.cmdInfo->legalOriginAddressMode = legalOriginAddressMode;
	reservedWordData->codeReservedWord.cmdInfo->legalDestAddressMode = legalDestAddressMode;
	reservedWordData->codeReservedWord.cmdInfo->numOfOperands = numOfOperands;

	return add_data_to_trie(db->trieDB, cmdName, reservedWordData);
}


/* get input string and ReservedWordDB, return TRUE if input is a reserved word */
bool is_reserved_word(ReservedWordDB* db, const char * input)
{
	return (get_data_from_trie(db->trieDB, input) != NULL);
}


/* get input string and ReservedWordDB, return ReservedWordType */
ReservedWordType get_reserved_word_type(ReservedWordDB* db, const char * input)
{
	ReservedWordData * reservedWordData =  get_data_from_trie(db->trieDB, input);
	return reservedWordData != NULL ? reservedWordData->type : TYPE_NOT_RESERVED;	
}


/* get input command name and ReservedWordDB, return CmdInfo in case command exists in database*/
CmdInfo * get_cmd_info(ReservedWordDB* db, const char * input)
{
  ReservedWordData * reservedWordData =  get_data_from_trie(db->trieDB, input);
  return ((reservedWordData != NULL) && (reservedWordData->type == TYPE_CMD)) ? reservedWordData->codeReservedWord.cmdInfo : NULL;
}


/* get input register name and ReservedWordDB, return Reg in case register exists in database */
Reg * get_reg_info(ReservedWordDB* db, const char * input)
{
	ReservedWordData * reservedWordData =  get_data_from_trie(db->trieDB, input);
    return ((reservedWordData != NULL) && (reservedWordData->type == TYPE_REG)) ? reservedWordData->codeReservedWord.reg : NULL;
}


/* deallocate memory for ReservedWordData */
void free_reserved_word_data(void * data)
{
	ReservedWordData * reservedWordData = (ReservedWordData*)data;

	switch(reservedWordData->type)
	{
		case TYPE_REG:
		{
			Reg * reg = reservedWordData->codeReservedWord.reg;
			free(reg);
			break;
		}
		case TYPE_CMD:
		{
			CmdInfo * cmdInfo = reservedWordData->codeReservedWord.cmdInfo;
			free(cmdInfo);
			break;
		}
	}

	free(reservedWordData);
}


/* deallocate memory for ReservedWordDB */
void free_reserved_word_db(ReservedWordDB * reservedWordDB)
{
	free_trie_db(reservedWordDB->trieDB, free_reserved_word_data);
	free(reservedWordDB);
}
