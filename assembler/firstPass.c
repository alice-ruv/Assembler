#include "firstPass.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "parseHandler.h"

state parse_input_p1(char * input, state currState, ProcessHandler * handler);
state handle_start_line_p1(char * firstWord, ProcessHandler * handler);
state handle_labeled_p1(const char * input, ProcessHandler * handler);
state handle_origin_operand_p1(const char * inputOperand, ProcessHandler * handler);
state handle_dest_operand_p1(const char * inputOperand, ProcessHandler * handler);
bool end_cmd_p1(ProcessHandler * handler);

bool validate_cmd_operand(const char * inputOperand, ProcessHandler * handler, unsigned int legalAddressMode);
state handle_comma(const char * input, ProcessHandler * handler);
state handle_data(const char * input, ProcessHandler * handler, bool isFirstVal);
state handle_string(const char * token, ProcessHandler * handler);

bool handle_label_declaration(char * firstWord, ProcessHandler * handler);
bool is_label_reserved_word(const char * inputLabel, ProcessHandler * handler);
state handle_label_extern(const char * labelName, ProcessHandler * handler);
bool is_legal_label_name(const char * name);
bool is_valid_label(const char * inputLabel, ProcessHandler * handler);


/* execute first pass of machine code construction and return TRUE if success */
bool execute_first_pass(FILE * inputFile, ProcessHandler * handler)
{
	/* init p1 functions to be used from parse handler, will be done one per program run*/
	if (handler->passFunctions->parseInput[P1] == NULL)
	{
		handler->passFunctions->parseInput[P1] = parse_input_p1;
		handler->passFunctions->endCmd[P1] = end_cmd_p1;
	}

	return parse_file(inputFile, handler);
}


/* parse input within the 1st pass and return our next state */
state parse_input_p1(char * input, state currState, ProcessHandler * handler)
{
	switch (currState)
	{
	case STATE_START_LINE:
		return handle_start_line_p1(input, handler);
	case STATE_LABELED:
		return handle_labeled_p1(input, handler);
	case STATE_CMD_ORIGIN_OPERAND:
		return handle_origin_operand_p1(input, handler);
	case STATE_CMD_DEST_OPERAND:
		return handle_dest_operand_p1(input, handler);
	case STATE_START_DATA:
		return handle_data(input, handler, TRUE);
	case STATE_CONTINUE_DATA:
		return handle_data(input, handler, FALSE);
	case STATE_COMMA:
		return handle_comma(input, handler);
	case STATE_STRING:
		return handle_string(input, handler);
	case STATE_EXTERN:
		return handle_label_extern(input, handler);
	default:
		return STATE_ERROR;
	}
}


/* handle comma input in 1st pass and return our next state */
state handle_comma(const char * input, ProcessHandler * handler)
{
	if (input[0] != ',')
	{
		printf("Error in file %s line %d: missing comma\n", handler->inputFileName, handler->lineNumber);
		return STATE_ERROR;
	}

	return (handler->cmd == NULL) ? STATE_CONTINUE_DATA : STATE_CMD_DEST_OPERAND;
}


/* handle string input in 1st pass and return our next state */
state handle_string(const char * token, ProcessHandler * handler)
{
	int i = 0;
	int firstApostropheIndex = -1;
	int lastApostropheIndex = -1;
	int lastNonWhiteChar = -1;

	for (; i < strlen(token); i++)
	{
		if (isspace(token[i]))
		{
			continue;
		}

		lastNonWhiteChar = i;

		if (token[i] == '\"' && firstApostropheIndex != -1)
		{
			lastApostropheIndex = i;
		}

		if (token[i] == '\"' && firstApostropheIndex == -1)
		{
			firstApostropheIndex = i;
		}

		if (token[i] != '\"' && firstApostropheIndex == -1)
		{
			printf("Error in file %s line %d: string should be declared within apostrophes\n",
					handler->inputFileName, handler->lineNumber);
			return STATE_ERROR;
		}
	} /*end of for*/

	if (firstApostropheIndex == -1 || lastApostropheIndex !=  lastNonWhiteChar)
	{
		printf("Error in file %s line %d: string should be declared within apostrophes\n",
				handler->inputFileName, handler->lineNumber);
		return STATE_ERROR;
	}

	/* the string we pass to dataImageDB is what we have between the first and last apostrophes*/
	if (add_string_to_db(handler->dataImageDB, token + firstApostropheIndex + 1,
			lastApostropheIndex - firstApostropheIndex - 1) == FALSE)
	{
		printf("Error in file %s line %d: string cannot be added to database\n", handler->inputFileName, handler->lineNumber);
		return STATE_ERROR;
	}

	/* if label was declared in current line, we are adding it to symbolDB with the address of the start of string with attribute data*/
	if (handler->labelName != NULL)
	{
		if (!add_symbol(handler->symbolDB, handler->labelName, handler->DC, ATT_DATA))
		{
			printf("Error in file %s line %d: failed adding data symbol \"%s\" to symbol table - label was already declared\n",
					handler->inputFileName, handler->lineNumber, handler->labelName);
			return STATE_ERROR;
		}
	}

	/* increasing DC by number of chars in the string + 1 for '/0'*/
	handler->DC += (lastApostropheIndex - firstApostropheIndex);
	return STATE_END_STRING;
} /*end of handle_string*/


/* handle end of instruction in 1st pass and return TRUE if success */
bool end_cmd_p1(ProcessHandler * handler)
{
	/* if label was declared in current line, we add this to symbolDB with the address of the start of string with attribute code*/
	if (handler->labelName != NULL)
	{

		if (add_symbol(handler->symbolDB, handler->labelName, handler->IC, ATT_CODE) == FALSE)
		{

			printf("Error in file %s line %d: failed adding %s to symbol table - label was already declared\n",
										handler->inputFileName, handler->lineNumber, handler->labelName);
			return FALSE;
		}
	}

	/* increase IC by the number of command words*/
	handler->IC += handler->cmd->cmdInfo->numOfOperands + 1;
	return TRUE;
}


/* handle start of line in 1st pass and return our next state according to the first string */
state handle_start_line_p1(char * firstWord, ProcessHandler * handler)
{
	/*in case of comment line we skip to next line*/
	if (firstWord[0] == ';')
		return STATE_SKIP_LINE;

	/*expect for directive line*/
	if (firstWord[0] == '.')
	{
		switch (get_reserved_word_type(handler->reservedWordDB, firstWord + 1))
		{
		case TYPE_DATA:
			return STATE_START_DATA;
		case TYPE_STRING:
			return STATE_STRING;
		case TYPE_EXTERN:
			return STATE_EXTERN;
		case TYPE_ENTRY:
			return STATE_SKIP_LINE;
		default:
			printf("Error in file %s line %d: undefined directive name \"%s\"\n", handler->inputFileName, handler->lineNumber, firstWord);
			return STATE_ERROR;
		}
	}

	/*expecting label declaration*/
	if (firstWord[strlen(firstWord) - 1] == ':')
	{
		return handle_label_declaration(firstWord, handler) ? STATE_LABELED : STATE_ERROR;
	}

	/*expect for instruction line (the remaining option)*/
	return handle_cmd_name(firstWord, handler);
}


/* handle word with data attribute (integer) in 1st pass and return our next state */
state handle_data(const char * input, ProcessHandler * handler, bool isFirstVal)
{
	int num;

	if (input[0] == ',' && strlen(input) == 1)
	{
		printf("Error in file %s line %d: multiple consecutive commas \n", handler->inputFileName, handler->lineNumber);
		return STATE_ERROR;
	}

	if (parse_number(input, &num) == FALSE)
	{
		printf("Error in file %s line %d: %s is not a valid number\n", handler->inputFileName, handler->lineNumber, input);
		return STATE_ERROR;
	}

	if (add_int_to_db(handler->dataImageDB, num) == FALSE)
	{
		printf("Error in file %s line %d: number cannot be added to database\n", handler->inputFileName, handler->lineNumber);
		return STATE_ERROR;
	}

	/* if label was declared in current line and it's the first number in the line
	 *  we are adding it to symbolDB with the address of the first number with attribute data*/
	if (handler->labelName != NULL && isFirstVal == TRUE)
	{
		if (!add_symbol(handler->symbolDB, handler->labelName, handler->DC, ATT_DATA))
		{
			printf("Error in file %s line %d: failed adding data symbol \"%s\" to symbol table - label was already declared\n",
										handler->inputFileName, handler->lineNumber, handler->labelName);
			return STATE_ERROR;
		}
	}
	handler->DC++;
	return STATE_COMMA;
}


/* handle origin operand of instruction in 1st pass and return our next state */
state handle_origin_operand_p1(const char * inputOperand, ProcessHandler * handler)
{
	return (validate_cmd_operand(inputOperand, handler, handler->cmd->cmdInfo->legalOriginAddressMode) == TRUE) ? STATE_COMMA : STATE_ERROR;
}


/* handle destination operand of instruction in 1st pass and return our next state */
state handle_dest_operand_p1(const char * inputOperand, ProcessHandler * handler)
{
	if (inputOperand[0] == ',' && strlen(inputOperand) == 1)
	{
		printf("Error in file %s line %d: multiple consecutive commas \n", handler->inputFileName, handler->lineNumber);
		return STATE_ERROR;
	}

	return (validate_cmd_operand(inputOperand, handler, handler->cmd->cmdInfo->legalDestAddressMode) == TRUE) ? STATE_END_LINE : STATE_ERROR;
}


/* get label name in 1st pass and return TRUE if name is legal */
bool is_legal_label_name(const char * name)
{
	int i;

	if (isalpha(name[0]) == FALSE)
	{
		return FALSE;
	}

	for (i = 1; i < strlen(name); i++)
	{
		if (isalnum(name[i]) == FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}


/* get command operand in 1st pass and return TRUE if operand is valid */
bool validate_cmd_operand(const char * inputOperand, ProcessHandler * handler, unsigned int legalAddressMode)
{
	switch (inputOperand[0])
	{
	/*expect for immediate addressing mode (number operand)*/
	case '#':
	{
		int number = 0;
		if ((legalAddressMode & IMMEDIATE_FLAG) == 0)
		{
			printf("Error in file %s line %d: current command cannot have immediate operand %s\n", handler->inputFileName, handler->lineNumber, inputOperand);
			return FALSE;
		}

		if (strlen(inputOperand) == 1)
		{
			printf("Error in file %s line %d: character # should be attached to number\n", handler->inputFileName, handler->lineNumber);
			return FALSE;
		}

		if (parse_number(inputOperand + 1, &number) == FALSE)
		{
			printf("Error in file %s line %d: %s is not a valid number\n", handler->inputFileName, handler->lineNumber, inputOperand + 1);
			return FALSE;
		}
		return TRUE;
	}
	/*expect for relative addressing mode (label operand which represents address of instruction in file)*/
	case '%':
	{
		if ((legalAddressMode & RELATIVE_FLAG) == 0)
		{
			printf("Error in file %s line %d: current command cannot have relative operand %s\n", handler->inputFileName, handler->lineNumber, inputOperand);
			return FALSE;
		}
		if (strlen(inputOperand) == 1)
		{
			printf("Error in file %s line %d: character %s should be attached to label\n", handler->inputFileName, handler->lineNumber, inputOperand);
			return FALSE;
		}

		if (is_legal_label_name(inputOperand + 1) == FALSE)
		{
			printf("Error in file %s line %d: \"%s\" is not a legal label name\n", handler->inputFileName, handler->lineNumber, inputOperand + 1);
			return FALSE;
		}
		return TRUE;
	}
	/*check for register-direct addressing mode (register operand)*/
	case 'r':
	{
		Reg * reg = get_reg_info(handler->reservedWordDB, inputOperand);

		if (reg != NULL)
		{
			if ((legalAddressMode & REGISTER_FLAG) == 0)
			{
				printf("Error in file %s line %d: current command cannot have register operand %s\n", handler->inputFileName, handler->lineNumber, inputOperand);
				return FALSE;
			}
			return TRUE;
		}
		/*the string starts with r but is not a register, check for label in default case*/
	}
	/*check for direct addressing mode (label operand)*/
	default:
		return is_valid_label(inputOperand, handler);

	}/*end of switch*/
}/*end of validate_cmd_operand*/


/* handle labeled directive/instruction in 1st pass and return our next state*/
state handle_labeled_p1(const char * input, ProcessHandler * handler)
{
	/*expect for directive line*/
	if (input[0] == '.')
	{
		ReservedWordType type;
		type = get_reserved_word_type(handler->reservedWordDB, input + 1);
		switch (type)
		{
		case TYPE_DATA:
			return STATE_START_DATA;
		case TYPE_STRING:
			return STATE_STRING;
		case TYPE_EXTERN:
		case TYPE_ENTRY:
		{
			if (handler->labelName != NULL)
			{
				printf("Warning in file %s line %d: label \"%s\" declared in the same line of extern/entry declaration\n",
						handler->inputFileName, handler->lineNumber, handler->labelName);
			}
			/*we are not handling Entry in the first pass*/
			return type == TYPE_EXTERN ? STATE_EXTERN : STATE_SKIP_LINE;
		}
		default:
			printf("Error in file %s line %d: undefined directive name \"%s\"\n", handler->inputFileName, handler->lineNumber, input);
			return STATE_ERROR;

		}/*end of switch*/
	}/*end of if*/

	/*expect for instruction line (the remaining option)*/
	return handle_cmd_name(input, handler);
}


/* handle label with extern attribute in 1st pass and return our next state*/
state handle_label_extern(const char * inputLabel, ProcessHandler * handler)
{
	SymbolData * symbolData;
	if (is_valid_label(inputLabel, handler) == FALSE)
	{
		return FALSE;
	}

	symbolData = try_get_symbol_data(handler->symbolDB, inputLabel);

	/*if symbol already declared, check for extern attribute*/
	if (symbolData != NULL)
	{
		if(symbolData->attributeFlags != ATT_EXTERN)
		{
			printf("Error in file %s line %d: label \"%s\" was already declared in symbol table without external flag\n",
													handler->inputFileName, handler->lineNumber, inputLabel);
		}

		return (symbolData->attributeFlags == ATT_EXTERN) ? STATE_END_LINE : STATE_ERROR;
	}

	if (!add_symbol(handler->symbolDB, inputLabel, 0, ATT_EXTERN))
	{
		printf("Error in file %s line %d: failed adding external symbol \"%s\" to symbol table - label was already declared\n", handler->inputFileName, handler->lineNumber, inputLabel);

		return STATE_ERROR;
	}
	return STATE_END_LINE;
}


/* get label name and return TRUE if name is valid */
bool is_valid_label(const char * inputLabel, ProcessHandler * handler)
{
	int labelSize = strlen(inputLabel);

	if (labelSize < 1)
	{
		printf("Error in file %s line %d: label must contain at least one character\n", handler->inputFileName, handler->lineNumber);
		return FALSE;
	}

	if (labelSize > MAX_LABEL_SIZE)
	{
		printf("Error in file %s line %d: label cannot contain more than %d characters\n", handler->inputFileName, handler->lineNumber, MAX_LABEL_SIZE);
		return FALSE;
	}

	if (is_label_reserved_word(inputLabel, handler))
	{
		printf("Error in file %s line %d: label name is not valid - \"%s\" is a reserved word\n", handler->inputFileName, handler->lineNumber, inputLabel);
		return FALSE;
	}

	if (is_legal_label_name(inputLabel) == FALSE)
	{
		printf("Error in file %s line %d: \"%s\" is not a legal label name\n", handler->inputFileName, handler->lineNumber, inputLabel);
		return FALSE;
	}

	return TRUE;
}


/* get label name and return TRUE if name is valid */
bool handle_label_declaration(char * inputLabel, ProcessHandler * handler)
{
	/*remove ':' from the end of the string*/
	inputLabel[strlen(inputLabel) - 1] = 0;

	if (is_valid_label(inputLabel, handler) == FALSE)
	{
		return FALSE;
	}

	handler->labelName = inputLabel;
	return TRUE;
}


/* get label and return TRUE if label name is a reserved word */
bool is_label_reserved_word(const char * inputLabel, ProcessHandler * handler)
{
	return is_reserved_word(handler->reservedWordDB, inputLabel);
}
