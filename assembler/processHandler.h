#include "codeImageDB.h"
#include "dataImageDB.h"
#include "reservedWordDB.h"
#include "symbolDB.h"
#include "externDB.h"
#include "entryDB.h"

#ifndef PROCESS_HANDLER_INCLUDED
#define PROCESS_HANDLER_INCLUDED

typedef enum {
	STATE_START_LINE,
	STATE_SKIP_LINE,
	STATE_LABELED,
	STATE_CMD_ORIGIN_OPERAND,
	STATE_CMD_DEST_OPERAND,
	STATE_COMMA,
	STATE_START_DATA,
	STATE_CONTINUE_DATA,
	STATE_STRING,
	STATE_END_STRING,
	STATE_ENTRY,
	STATE_EXTERN,
	STATE_END_LINE,
	STATE_ERROR
} state;  /*input parsing will be done according to current state in every pass*/

typedef struct ProcessHandler ProcessHandler;

/*input parsing will be done according to 1st/2nd pass*/
typedef enum {P1, P2, NUM_PASSES} Pass;

typedef  state (* parse_input)(char * , state , ProcessHandler * );
typedef bool (* end_cmd)(ProcessHandler *);

/*Arrays of function pointers. Each array is size of 2, per pass*/
typedef struct
{
	parse_input * parseInput;
	end_cmd * endCmd;
}PassFunctions;

struct ProcessHandler
{
	/** per entire program **/
	PassFunctions * passFunctions;
	ReservedWordDB * reservedWordDB;

	/** for the entire file* */
	Pass pass ;
	const char * inputFileName;
	const char * fileNamePrefix;
	int IC; /*Instruction-Counter*/
	int DC; /*Data-Counter*/
	int ICF; /*final value of IC*/
	SymbolDB * symbolDB;
	CodeImageDB * codeImageDB;
	DataImageDB * dataImageDB;
	ExternDB * externDB;
	EntryDB *  entryDB;
	
	/** per line **/
	int lineNumber;
	const char * labelName; /*labelName is allocated on the stack, will not be released in free function*/
	Cmd * cmd;
	char * commaHandleInput;
};


void init_handler_for_line(ProcessHandler * handler);
void free_handler_data_per_line(ProcessHandler * handler);
void free_memory(ProcessHandler * handler);


#endif
