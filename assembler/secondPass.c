#include "secondPass.h"
#include "parseHandler.h"
#include "assemblerDefinitions.h"
#include <string.h>

state parse_input_p2(char * input, state currState, ProcessHandler * handler);
state handle_start_line_p2(const char * firstWord, ProcessHandler * handler);
state handle_labeled_p2(const char * input, ProcessHandler * handler);
state handle_origin_operand_p2(const char * inputOperand, ProcessHandler * handler);
state handle_dest_operand_p2(const char * inputOperand, ProcessHandler * handler);
bool end_cmd_p2(ProcessHandler * handler);

bool handle_cmd_operand(const char * inputOperand, ProcessHandler * handler, unsigned int legalAddressMode, int ic_offset,
		unsigned int * out_addressMode, CodeWord ** out_memoryWord);

state handle_entry(const char * input, ProcessHandler * handler);
CodeWord * create_memory_word(unsigned int code, AREType type);


/* execute second pass of machine code construction and return TRUE if success */
bool execute_second_pass(FILE * inputFile, ProcessHandler * handler)
{
	/* init p2 functions to be used from parse handler. will be done one per program run*/
	if (handler->passFunctions->parseInput[P2] == NULL)
	{
		handler->passFunctions->parseInput[P2] = parse_input_p2;
		handler->passFunctions->endCmd[P2] = end_cmd_p2;
	}

	return parse_file(inputFile, handler);
}


/* parse input within the 2st pass and return our next state */
state parse_input_p2(char * input, state currState, ProcessHandler * handler)
{
	switch (currState)
	{
	case STATE_START_LINE:
		return handle_start_line_p2(input, handler);
	case STATE_LABELED:
		return handle_labeled_p2(input, handler);
	case STATE_CMD_ORIGIN_OPERAND:
		return handle_origin_operand_p2(input, handler);
	case STATE_CMD_DEST_OPERAND:
		return handle_dest_operand_p2(input, handler);
	case STATE_COMMA:
		return STATE_CMD_DEST_OPERAND; /*in second pass we can reach from comma only to destination operand of instruction*/
	case STATE_ENTRY:
		return handle_entry(input, handler);
	default:
		return STATE_ERROR;
	}

}


/* handle entry input in 2nd pass and return our next state */
state handle_entry(const char * input, ProcessHandler * handler)
{
	SymbolData * symbolData;
	int labelSize = strlen(input);
	char * inputLabel;
	EntryData * entryData;
	symbolData = try_get_symbol_data(handler->symbolDB, input);

	if (symbolData == NULL)
	{
		printf("Error in file %s line %d: \"%s\" wasn't found in symbol table - label wasn't declared \n", handler->inputFileName, handler->lineNumber, input);
		return STATE_ERROR;
	}

	if ((symbolData->attributeFlags & ATT_EXTERN) != 0)
	{
		printf("Error in file %s line %d: label \"%s\" cannot be declared both as external and as entry\n", handler->inputFileName, handler->lineNumber, input);
		return STATE_ERROR;
	}

	if ((symbolData->attributeFlags & ATT_ENTRY) != 0)
	{
		/* label already was declared as entry, we can ignore this line*/
		return STATE_END_LINE;
	}

	symbolData->attributeFlags = symbolData->attributeFlags | ATT_ENTRY;

	entryData = check_memory_alloc(malloc (sizeof(EntryData)));
	inputLabel = check_memory_alloc(calloc(labelSize + 1, sizeof(char)));

	strncpy(inputLabel, input, labelSize);
	entryData->symbolName = inputLabel;
	entryData->address = ((symbolData->attributeFlags & ATT_DATA) != 0) ? symbolData->value + handler->ICF : symbolData->value;

	if (add_entry_to_db(handler->entryDB, entryData) == FALSE)
	{
		printf("Error in file %s line %d: failed adding entry symbol \"%s\" to database\n", handler->inputFileName, handler->lineNumber, input);
		free(inputLabel);
		free(entryData);
		return STATE_ERROR;
	}

	return STATE_END_LINE;
}


/* end command input line in 2nd pass and return TRUE if success */
bool end_cmd_p2(ProcessHandler * handler)
{
	CodeWord * memoryWord;

	/*prepare first code word of command*/
	int word = handler->cmd->cmdInfo->optcode;
	word = (word << FUNCT_SIZE) + handler->cmd->cmdInfo->funct;
	word = (word << ADDRESS_SIZE) + handler->cmd->originAddressMode;
	word = (word << ADDRESS_SIZE) + handler->cmd->destAddressMode;

	memoryWord = create_memory_word(word, TYPE_A);
	handler->cmd->cmdWords[0] = memoryWord;

	if (add_cmd_to_db(handler->codeImageDB, handler->cmd) == FALSE)
	{
		printf("Error in file %s line %d: failed adding command to database\n", handler->inputFileName, handler->lineNumber);
		return FALSE;
	}

	/* increasing IC by the number command words*/
	handler->IC += handler->cmd->cmdInfo->numOfOperands + 1;
	return TRUE;
}


/* create CodeWord in 2nd pass */
CodeWord * create_memory_word(unsigned int code, AREType type)
{
	CodeWord * memoryWord = check_memory_alloc(malloc(sizeof(CodeWord)));
	memoryWord->codeRep = code;
	memoryWord->type = type;
	return memoryWord;
}


/* handle first string in input line in 2nd pass and return our next state*/
state handle_start_line_p2(const char * firstWord, ProcessHandler * handler)
{
	/*in case of comment line we skip to next line*/
	if (firstWord[0] == ';')
		return STATE_SKIP_LINE;

	/*expect for directive line. we only handle entry type in 2nd pass. other cases were handled in 1st pass*/
	if (firstWord[0] == '.')
	{
		switch (get_reserved_word_type(handler->reservedWordDB, firstWord + 1))
		{
		case TYPE_ENTRY:
			return STATE_ENTRY;
		case TYPE_DATA:
		case TYPE_STRING:
		case TYPE_EXTERN:
			return STATE_SKIP_LINE;
		default:
			return STATE_ERROR;
		}
	}

	/*expecting label declaration*/
	if (firstWord[strlen(firstWord) - 1] == ':') {
		return STATE_LABELED;
	}

	/*expect for instruction line (the remaining option)*/
	return handle_cmd_name(firstWord, handler);
}


/* handle command input operand and return true if operand is valid */
bool handle_cmd_operand(const char * inputOperand, ProcessHandler * handler, unsigned int legalAddressMode, int ic_offset,
		unsigned int * out_addressMode, CodeWord ** out_memoryWord)
{
	/*most of the cases were checked for validity in 1st pass, so we don't need to check again for the same errors */
	switch (inputOperand[0])
	{
	/*expect for immediate addressing mode (number operand)*/
	case '#':
	{
		int number = 0;
		parse_number(inputOperand + 1, &number);
		(*out_memoryWord) = create_memory_word(number, TYPE_A);
		(*out_addressMode) = IMMEDIATE;
		return TRUE;
	}
	/*expect for relative addressing mode (label operand which represents address of instruction in file)*/
	case '%':
	{
		SymbolData * symbolData;
		symbolData = try_get_symbol_data(handler->symbolDB, inputOperand + 1);
		if (symbolData == NULL)
		{																			/*XXX*/
			printf("Error in file %s line %d: \"%s\" wasn't found in symbol table - label wasn't declared\n", handler->inputFileName, handler->lineNumber, inputOperand + 1);
			return FALSE;
		}
		if ((symbolData->attributeFlags & ATT_EXTERN) != 0)
		{
			printf("Error in file %s line %d: \"%s\" cannot be declared as external because was declared earlier\n", handler->inputFileName,
					handler->lineNumber, inputOperand + 1);
			return FALSE;
		}
		if ((symbolData->attributeFlags & ATT_CODE) == 0) {
			printf("Error in file %s line %d: label \"%s\" wasn't declared in instruction line\n", handler->inputFileName, handler->lineNumber, inputOperand+1);
			return FALSE;
		}
		(*out_memoryWord) = create_memory_word(symbolData->value - (handler->IC + ic_offset), TYPE_A);
		(*out_addressMode) = RELATIVE;
		return TRUE;
	}
	/*check for register-direct addressing mode (register operand)*/
	case 'r':
	{
		Reg * reg = get_reg_info(handler->reservedWordDB, inputOperand);
		if (reg != NULL)
		{
			(*out_memoryWord) = create_memory_word(reg->regRep, TYPE_A);
			(*out_addressMode) = REGISTER_DIRECT;
			return TRUE;
		}
		/*the string starts with r but is not a register, check for label in default case*/
	}
	/*check for direct addressing mode (label operand)*/
	default:
	{
		SymbolData * symbolData;
		symbolData = try_get_symbol_data(handler->symbolDB, inputOperand);
		if (symbolData == NULL)
		{																			/*XXX*/
			printf("Error in file %s line %d: \"%s\" wasn't found in symbol table - label wasn't declared\n", handler->inputFileName, handler->lineNumber, inputOperand);
			return FALSE;
		}
		/*check for extern attribute of input label*/
		if ((symbolData->attributeFlags & ATT_EXTERN) != 0)
		{
			int labelSize = strlen(inputOperand);
			char * inputLabel = check_memory_alloc(calloc(labelSize + 1, sizeof(char)));
			ExternData * externData = check_memory_alloc(malloc(sizeof(ExternData)));
			strncpy(inputLabel, inputOperand, labelSize);
			externData->address = handler->IC + ic_offset;
			externData->symbolName = inputLabel;

			if ((add_extern_to_db(handler->externDB, externData) == FALSE))
			{
				printf("Error in file %s line %d: failed adding external symbol \"%s\" to database\n", handler->inputFileName, handler->lineNumber, inputOperand);
				return FALSE;
			}
			(*out_memoryWord) = create_memory_word(0, TYPE_E);
		}
		/*in this case addressing mode will be direct and we checked above that ARE type is not E, so it should be R*/
		/*check for data attribute of input label*/
		else if ((symbolData->attributeFlags & ATT_DATA) != 0)
		{
			(*out_memoryWord) = create_memory_word(symbolData->value + handler->ICF, TYPE_R);
		}
		/*it's code attribute of input label*/
		else
		{
			(*out_memoryWord) = create_memory_word(symbolData->value, TYPE_R);
		}

		(*out_addressMode) = DIRECT;
		return TRUE;
	}
	}/*end of switch*/
} /*end of handle_cmd_operand*/


/* handle command origin operand in 2nd pass and return our next state */
state handle_origin_operand_p2(const char * inputOperand, ProcessHandler * handler)
{
	unsigned int addressMode;
	CodeWord * memoryWord;
	if (handle_cmd_operand(inputOperand, handler, handler->cmd->cmdInfo->legalOriginAddressMode, 1, &addressMode, &memoryWord) == FALSE)
	{
		return STATE_ERROR;
	}

	handler->cmd->originAddressMode = addressMode;
	handler->cmd->cmdWords[1] = memoryWord;

	return STATE_COMMA;
}


/* handle command destination operand in 2nd pass and return our next state */
state handle_dest_operand_p2(const char * inputOperand, ProcessHandler * handler)
{
	unsigned int addressMode;
	CodeWord * memoryWord;

	if (handle_cmd_operand(inputOperand, handler, handler->cmd->cmdInfo->legalDestAddressMode, handler->cmd->cmdInfo->numOfOperands,
			&addressMode, &memoryWord) == FALSE)
	{
			return STATE_ERROR;
	}

	handler->cmd->destAddressMode = addressMode;
	handler->cmd->cmdWords[handler->cmd->cmdInfo->numOfOperands] = memoryWord;

	return STATE_END_LINE;
}


/* handle labeled instruction in 2nd pass and return our next state */
state handle_labeled_p2(const char * input, ProcessHandler * handler)
{
	if (input[0] == '.')
	{
		return  (get_reserved_word_type(handler->reservedWordDB, input + 1) == TYPE_ENTRY) ? STATE_ENTRY : STATE_SKIP_LINE;

	}

	return  handle_cmd_name(input, handler);
}
