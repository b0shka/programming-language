#include "main.h"
#include "parser.hpp"

extern FILE *yyin;

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cout << "Provide a filename to parse!" << std::endl;
		exit(1);
	}
	FILE *sourceFile = fopen(argv[1], "r");

	if (!sourceFile) {
		std::cout << "Could not open source file " << argv[1] << std::endl;
		exit(1);
	}

	yyin = sourceFile;
	return yyparse();
}
