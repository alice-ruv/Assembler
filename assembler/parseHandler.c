#include <string.h>
#include <ctype.h>
#include "parseHandler.h"

char * get_next_token(ProcessHandler * handler, char * currToken, state * in_out_state);
bool get_line (FILE * inputFile, char * out_line);
char * get_first_token(char * inputLine);


/* prepare next line for parsing and return TRUE if success
 * return FALSE if we reached to EOF */
bool get_line (FILE * inputFile, char * out_line)
{
	int inputSize;

	if (fgets(out_line, LINE_SIZE, inputFile) == NULL) {  /*reach to end-of-file*/
		return FALSE;
	}

	inputSize = strlen(out_line);

	/*remove \n from the input*/
	if (out_line[inputSize - 1] == '\n') {
		out_line[inputSize - 1] = '\0';
		inputSize--;
	}

	return TRUE;
}


/* return first token for parsing */
char * get_first_token(char * inputLine)
{
	const char * delim = " \t\n\r\v\f";  /*input delimited by white characters*/
	return strtok(inputLine, delim);
}


/* parse input file and return TRUE in case that errors wasn't found*/
bool parse_file(FILE * inputFile, ProcessHandler * handler)
{
	/*inErrorState will be set to TRUE in case we reached to error in input file.
	* In this case we know than we don't have to create output files, or if we are in first pass we won't continue to the 2nd pass,
	* but we will continue check for more errors to print in the current pass*/
	bool inErrorState = FALSE;
	handler->lineNumber = 0;

	FOREVER
	{
		char *token = NULL;
		state currState = STATE_START_LINE;
		char inputLine[LINE_SIZE] = { 0 };

		if (get_line(inputFile, inputLine) == FALSE)
		{
			return !inErrorState;
		}

		init_handler_for_line(handler);

		if ((token = get_first_token(inputLine)) == NULL)
		{
			continue;  /*if line is empty continue to the next line*/
		}

		while (token != NULL)
		{
			/*parse input according to 1st/2nd pass*/
			currState = handler->passFunctions->parseInput[handler->pass](token, currState, handler);

			token = get_next_token(handler, token, &currState);

			if (currState == STATE_ERROR)
			{
				inErrorState = TRUE;
				break;
			}
		}

		switch(currState)
		{
			case STATE_LABELED:
			{
				printf("Error in file %s line %d: missing instruction/ directive after label declaration \n", handler->inputFileName, handler->lineNumber);
				break;
			}
			case STATE_CMD_ORIGIN_OPERAND:
			{
				printf("Error in file %s line %d: missing operands \n", handler->inputFileName, handler->lineNumber);
				currState = STATE_ERROR;
				break;
			}
			case STATE_CMD_DEST_OPERAND:
			{
				printf("Error in file %s line %d: missing operand \n", handler->inputFileName, handler->lineNumber);
				currState = STATE_ERROR;
				break;
			}
			case STATE_COMMA:
			{
				if (handler->cmd != NULL)
				{
					printf("Error in file %s line %d: missing comma and second operand\n", handler->inputFileName, handler->lineNumber);
					currState = STATE_ERROR;
				}
				break;
			}
			case STATE_START_DATA:
			{
				printf("Error in file %s line %d: missing value/s after \".data\"\n", handler->inputFileName, handler->lineNumber);
				currState = STATE_ERROR;
				break;
			}
			case STATE_CONTINUE_DATA:
			{
				printf("Error in file %s line %d: extraneous text after end of instruction \n",
								handler->inputFileName, handler->lineNumber);
				currState = STATE_ERROR;
				break;
			}
			case STATE_STRING:
			{
				printf("Error in file %s line %d: missing string after \".string\"\n", handler->inputFileName, handler->lineNumber);
				currState = STATE_ERROR;
				break;
			}
			case STATE_ENTRY:
			{
				printf("Error in file %s line %d: missing label after \".entry\"\n", handler->inputFileName, handler->lineNumber);
				currState = STATE_ERROR;
				break;
			}
			case STATE_EXTERN:
			{
				printf("Error in file %s line %d: missing label after \".extern\"\n", handler->inputFileName, handler->lineNumber);
				currState = STATE_ERROR;
				break;
			}
			default:
			{
				break;
			}

		}

		if (currState == STATE_ERROR)
		{
			inErrorState = TRUE;
		}

		if (currState != STATE_ERROR && handler->cmd != NULL)
		{
			if (handler->passFunctions->endCmd[handler->pass](handler) == FALSE && inErrorState == FALSE )
			{
				inErrorState = TRUE;
			}
		}

		free_handler_data_per_line(handler);

		}/*end of FOREVER*/

		return !inErrorState;

} /*end of parse_file*/


/* return next token for parsing */
char * get_next_token(ProcessHandler * handler, char * currToken, state * in_out_state)
{
	const char * delim = " \t\n\r\v\f";
	char * nextToken = currToken;

	if (*in_out_state == STATE_ERROR)
	{
		return NULL;
	}
	if (*in_out_state == STATE_SKIP_LINE || *in_out_state == STATE_END_STRING)
	{
		*in_out_state = STATE_END_LINE;
		return NULL;
	}
	if (*in_out_state == STATE_STRING)
	{
		nextToken =  strtok(NULL, "");
	}
	/*Two cases for handling input separated by comma:
	 * 1st case: command with 2 operands (must have origin operand)
	 * 2nd case: 2 or more parameters after directive ".data"*/
	else if (*in_out_state == STATE_CMD_ORIGIN_OPERAND || *in_out_state == STATE_START_DATA)
	{
		int j = 0;
		char* input = currToken + strlen(currToken) + 1;
		handler->commaHandleInput = check_memory_alloc(calloc(LINE_SIZE * LINE_SIZE_MUL_FACTOR, sizeof(char)));

		/*separate all commas in input by white spaces for tokenize*/
		for (; *input != '\0'; input++)
		{
			if (*input == ',') {
				handler->commaHandleInput[j] = ' ';
				j++;
				handler->commaHandleInput[j] = ',';
				j++;
				handler->commaHandleInput[j] = ' ';
			} else {
				handler->commaHandleInput[j] = *input;
			}

			j++;
		}

		nextToken = strtok(handler->commaHandleInput, delim);
	} else {
		nextToken = strtok(NULL, delim); /*continue tokenize rest of the line*/
	}

	/* for .data after each number we expect comma or end of line */
	if (nextToken == NULL && *in_out_state == STATE_COMMA && handler->cmd == NULL)
	{
		*in_out_state = STATE_END_LINE;
		return NULL;
	}

	if (*in_out_state == STATE_END_LINE && nextToken != NULL)
	{
		*in_out_state =  STATE_ERROR;
		printf("Error in file %s line %d: extraneous text after end of instruction/ directive\n",
				handler->inputFileName, handler->lineNumber);
		return NULL;
	}

	return nextToken;
}


/* parse input number and place it in out_num parameter, return TRUE if success */
bool parse_number(const char * valInput, int * out_num)
{
	int val = 0;
	int i = 0, sign = 1, inputSize;
	bool hasDigit = FALSE;
	inputSize = strlen(valInput);

	sign = (valInput[i] == '-') ? -1 : 1;

	if (valInput[i] == '+' || valInput[i] == '-')
		i++;

	for (; i < inputSize; i++)
	{
		if (isdigit(valInput[i]) == FALSE)
		{
			return FALSE;
		}
		val = 10 * val + (valInput[i] - '0');
		hasDigit = TRUE;
	}

	if ((hasDigit == FALSE))
	{
		return FALSE;
	}

	val = val * sign;
	(*out_num) = val;
	return TRUE;
}


/* handle input command name (create Cmd) and return our next state  */
state handle_cmd_name(const char * input, ProcessHandler * handler)
{
	CmdInfo * cmdInfo = get_cmd_info(handler->reservedWordDB, input);

	if (cmdInfo == NULL)
	{
		printf("Error in file %s line %d: undefined command name \"%s\"\n", handler->inputFileName, handler->lineNumber, input);
		return STATE_ERROR;
	}

	handler->cmd = check_memory_alloc(malloc(sizeof(Cmd)));
	handler->cmd->cmdInfo = cmdInfo;
	handler->cmd->destAddressMode = UNUSED;
	handler->cmd->originAddressMode = UNUSED;
	handler->cmd->cmdWords = (handler->pass == P1) ?
			NULL : check_memory_alloc(calloc(cmdInfo->numOfOperands + 1, sizeof(CodeWord*)));


	switch (handler->cmd->cmdInfo->numOfOperands)
	{
	case NO_OPERANDS:
		return STATE_END_LINE;
	case ONE_OPERAND:
		return STATE_CMD_DEST_OPERAND;
	case TWO_OPERANDS:
		return STATE_CMD_ORIGIN_OPERAND;
	default:
		return STATE_ERROR;
	}
}
