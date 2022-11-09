#include "main.h"
#include "parser.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Provide a filename to parse!\n");
		exit(1);
	}
	FILE *sourceFile = fopen(argv[1], "r");

	if (!sourceFile) {
		printf("Could not open source file %s\n", argv[1]);
		exit(1);
	}

	yyin = sourceFile;
	return yyparse();
}
