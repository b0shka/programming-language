#include "main.h"
#include "parser.hpp"

extern FILE *yyin;

int main(int argc, char **argv) {
	if (argc < 2) {
		cout << "Provide a filename to parse!" << endl;
		exit(1);
	}
	FILE *sourceFile = fopen(argv[1], "r");

	if (!sourceFile) {
		cout << "Could not open source file " << argv[1] << endl;
		exit(1);
	}

	yyin = sourceFile;
	return yyparse();
}
