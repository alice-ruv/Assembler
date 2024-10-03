all : assembler

assembler : assembler.o firstPass.o secondPass.o fileHandler.o utils.o processHandler.o linkedList.o trieDB.o reservedWordDB.o symbolDB.o codeImageDB.o dataImageDB.o outputHandler.o parseHandler.o externDB.o entryDB.o assemblerHandler.o
	gcc -g  -ansi -Wall -pedantic assembler.o firstPass.o secondPass.o fileHandler.o utils.o processHandler.o linkedList.o trieDB.o reservedWordDB.o symbolDB.o codeImageDB.o dataImageDB.o outputHandler.o parseHandler.o externDB.o entryDB.o assemblerHandler.o -o assembler

assembler.o : assembler.c processHandler.h 
	gcc -g -c -ansi -Wall -pedantic assembler.c -o assembler.o

firstPass.o : firstPass.c firstPass.h 
	gcc -g -c -ansi -Wall -pedantic firstPass.c -o firstPass.o
        
secondPass.o : secondPass.c secondPass.h 
	gcc -g -c -ansi -Wall -pedantic secondPass.c -o secondPass.o 
	
fileHandler.o : fileHandler.c fileHandler.h
	gcc -g -c -ansi -Wall -pedantic fileHandler.c -o fileHandler.o
	
utils.o : utils.c utils.h
	gcc -g -c -ansi -Wall -pedantic utils.c -o utils.o
	
processHandler.o : processHandler.c processHandler.h
	gcc -g -c -ansi -Wall -pedantic processHandler.c -o processHandler.o

linkedList.o : linkedList.c linkedList.h
	gcc -g -c -ansi -Wall -pedantic linkedList.c -o linkedList.o
	
trieDB.o : trieDB.c trieDB.h
	gcc -g -c -ansi -Wall -pedantic trieDB.c -o trieDB.o
	
reservedWordDataBase.o : reservedWordDB.c reservedWordDB.h
	gcc -g -c -ansi -Wall -pedantic reservedWordDB.c -o reservedWordDB.o
	
symbolDB.o : symbolDB.c symbolDB.h 
	gcc -g -c -ansi -Wall -pedantic symbolDB.c -o symbolDB.o
	
codeImageDB.o : codeImageDB.c codeImageDB.h
	gcc -g -c -ansi -Wall -pedantic codeImageDB.c -o codeImageDB.o
	
dataImageDB.o : dataImageDB.c dataImageDB.h
	gcc -g -c -ansi -Wall -pedantic dataImageDB.c -o dataImageDB.o
	
outputHandler.o : outputHandler.c outputHandler.h
	gcc -g -c -ansi -Wall -pedantic outputHandler.c -o outputHandler.o
	
parseHandler.o : parseHandler.c parseHandler.h
	gcc -g -c -ansi -Wall -pedantic parseHandler.c -o parseHandler.o
	
externDB.o : externDB.c externDB.h
	gcc -g -c -ansi -Wall -pedantic externDB.c -o externDB.o
  
entryDB.o : entryDB.c entryDB.h
	gcc -g -c -ansi -Wall -pedantic entryDB.c -o entryDB.o
	
assemblerHandler.o : assemblerHandler.c assemblerHandler.h
	gcc -g -c -ansi -Wall -pedantic assemblerHandler.c -o assemblerHandler.o
	     
clean:
	rm -f assembler *.o 

