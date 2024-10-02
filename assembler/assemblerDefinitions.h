#ifndef ASSEMBLER_DEFINITIONS_INCLUDED
#define ASSEMBLER_DEFINITIONS_INCLUDED

#define LINE_SIZE 82
#define MAX_LABEL_SIZE 31
#define NUM_OF_REGISTERS 8
#define WORD_SIZE 12
#define OPTCODE_SIZE 4
#define FUNCT_SIZE 4
#define ADDRESS_SIZE 2
#define LINE_SIZE_MUL_FACTOR 3
#define FIRST_ADDRESS 100

/** memory image (code and data) definitions **/

/*every word in machine-code will have one of the following types: A - Absolute, R - Relocatable, E - External*/
typedef enum {TYPE_A = 'A', TYPE_R = 'R', TYPE_E = 'E'} AREType;


/** code image definitions **/

/*every command operand will have one of the following addressing mode
 *in case operand does not exist addressModeFlag will be set to NO_OPERAND_FLAG*/
typedef enum {NO_OPERAND_FLAG = 0x0, IMMEDIATE_FLAG = 0x1, DIRECT_FLAG = 0x2, RELATIVE_FLAG = 0x4, REGISTER_FLAG = 0x8} addressModeFlag;

/*every operand of command will have one of the following addressing mode: Immediate, Direct, Relative, Register-direct*/
typedef enum {UNUSED = 0, IMMEDIATE = 0, DIRECT, RELATIVE, REGISTER_DIRECT, NUM_ADDRESS_MODE} addressMode;

/*every command will have 0-2 operands*/
typedef enum {NO_OPERANDS, ONE_OPERAND, TWO_OPERANDS} NumOfOperands;

typedef struct
{
	unsigned int codeRep : WORD_SIZE;
	AREType type;
}CodeWord;

typedef struct
{
	unsigned int optcode : OPTCODE_SIZE;
	unsigned int funct : FUNCT_SIZE;
	unsigned int legalOriginAddressMode : NUM_ADDRESS_MODE;
	unsigned int legalDestAddressMode : NUM_ADDRESS_MODE;
	NumOfOperands numOfOperands;
}CmdInfo;

typedef struct
{
	CmdInfo* cmdInfo;
	CodeWord ** cmdWords; /*Array of code words. Array size is number of operands + 1 (first word with the command ) */
	unsigned int originAddressMode : ADDRESS_SIZE;
	unsigned int destAddressMode : ADDRESS_SIZE;
}Cmd;

typedef struct
{
	unsigned int regRep : WORD_SIZE;
}Reg;


/** data image definitions **/

typedef struct
{
	AREType type;
	unsigned int dataRep : WORD_SIZE;
}DataWord;


/** symbol definitions **/

typedef struct
{
	int address;
	const char * symbolName;
}ExternData;

/*the structure of ExternData is the same as in EntryData, so we just used typedef*/
typedef ExternData EntryData;


/*every symbol in symbolDB has at least one of the following attributes */
typedef enum {ATT_CODE = 0x1, ATT_DATA = 0x2, ATT_ENTRY = 0x4, ATT_EXTERN = 0x8} attributeFlag;

typedef struct
{
		int value;
		unsigned int attributeFlags : 4;
}SymbolData;


#endif
