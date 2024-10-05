# Assembler
An assembler program that translates custom assembly statements into machine code, processed by a custom CPU with defined registers and RAM.

## Technology Stack
* **Language:** C
* **Platform:** Linux
* **Tools:** Custom CPU simulator, GCC (GNU Compiler Collection)
* **Key Concepts:** Assembly language, Machine code, CPU architecture, Memory management (registers and RAM)

## Flowchart

[main()](https://github.com/alice-ruv/Assembler/blob/main/assembler/assembler.c#L6)  gets file names from the command line   
   ↓     
[process_files()](https://github.com/alice-ruv/Assembler/blob/main/assembler/assemblerHandler.c#L88) processes every input file with the `.as` extension  
   ↓    
[init_reserved_word_db()](https://github.com/alice-ruv/Assembler/blob/main/assembler/reservedWordDB.c#L32) initializes the reserved words database, which is used during the input parsing  
   ↓  
[execute_first_pass()](https://github.com/alice-ruv/Assembler/blob/main/assembler/firstPass.c#L27)  
      ↓  
If valid:  
   - Create output file: `file.ob`  
   - Optionally create output files: `file.ent`, `file.ext`   
   ↓  
[execute_second_pass()](https://github.com/alice-ruv/Assembler/blob/main/assembler/secondPass.c#L21) completes filling the output files

