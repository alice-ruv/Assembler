#include <stdio.h>
#include <stdlib.h>
#include "assemblerHandler.h"

/* make file/s that contain machine code from input file/s written in assembly */
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		puts("Numbers of arguments to the program should be at least 1.");
		return 1;
	}

	if (process_files(argv + 1, argc - 1) == FALSE)
	{
		puts("Assembler failed to perform execution.");
		return 1;
	}

	return 0;
}
