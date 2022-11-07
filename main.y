%{
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

extern int yylex();
extern int yywrap();
void yyerror (char *s);

int variables[52];
int getValueID(char id);
void assigmentValueID(char id, int val);
void assigmentUnaryValueID(char id, int val, bool isAdd);
void unaryID(char id, bool isAdd);
%}

%union {
	char id;
	int num;
	float float_num;
	char *string;
	char charaster;
}

%token PRINT ADD SUBTRACT MULTIPLY DIVIDE INCREMENT DECREMENT ADDASSIGMENT SUBTRACTASSIGMENT
%token <id> ID
%token <num> NUMBER
%token <float_num> FLOAT_NUM
%token <string> STRING
%token <charaster> CHARACTER
%type <num> exp term
%type <id> assignment
%left '+' '-'
%left '*' '/'

%%

program		:	PRINT '(' exp ')' ';'				{ printf("%d\n", $3); }
			|	program PRINT '(' exp ')' ';'		{ printf("%d\n", $4); }

			|	PRINT '(' FLOAT_NUM ')' ';'			{ printf("%f\n", $3); }
			|	program PRINT '(' FLOAT_NUM ')' ';'	{ printf("%f\n", $4); }

			|	PRINT '(' STRING ')' ';'			{ printf("%s\n", $3); }
			|	program PRINT '(' STRING ')' ';'	{ printf("%s\n", $4); }

			|	PRINT '(' CHARACTER ')' ';'			{ printf("%c\n", $3); }
			|	program PRINT '(' CHARACTER ')' ';'	{ printf("%c\n", $4); }		

			|	assignment ';'						{ ; }
			|	program assignment ';'				{ ; }
			|	exp ';'								{ ; }
			|	program exp ';'						{ ; }
			|	FLOAT_NUM ';'						{ ; }
			|	program FLOAT_NUM ';'				{ ; }
			|	STRING ';'							{ ; }
			|	program STRING ';'					{ ; }
			|	CHARACTER ';'						{ ; }
			|	program CHARACTER ';'				{ ; }
			;

assignment	:	ID '=' exp							{ assigmentValueID($1, $3); }
			|	ID ADDASSIGMENT exp					{ assigmentUnaryValueID($1, $3, true); }
			|	ID SUBTRACTASSIGMENT exp			{ assigmentUnaryValueID($1, $3, false); }
			|	ID INCREMENT						{ unaryID($1, true); }
			|	ID DECREMENT						{ unaryID($1, false); }
			;

exp			:	term								{ $$ = $1; }
			|	exp ADD term						{ $$ = $1 + $3; }
			|	exp SUBTRACT term					{ $$ = $1 - $3; }
			//|	exp MULTIPLY term					{ $$ = $1 * $3; }
			//|	exp DIVIDE term						{ $$ = $1 / $3; }
			;

term		:	'(' exp ')'							{ $$ = $2; }
			|	SUBTRACT term						{ $$ = -$2; }
			|	NUMBER								{ $$ = $1; }
			|	ID									{ $$ = getValueID($1); }
			;

%%

int computeIDIndex(char id) {
	int idx = -1;
	if (islower(id)) {
		idx = id - 'a' + 26;
	} else if (isupper(id)) {
		idx = id - 'A';
	}
	return idx;
} 

int getValueID(char id) {
	int bucket = computeIDIndex(id);
	return variables[bucket];
}

void assigmentValueID(char id, int val) {
	int bucket = computeIDIndex(id);
	variables[bucket] = val;
}

void assigmentUnaryValueID(char id, int val, bool isAdd) {
	int bucket = computeIDIndex(id);
	int value = getValueID(id);
	variables[bucket] = isAdd ? value + val : value - val;
}

void unaryID(char id, bool isAdd) {
	int bucket = computeIDIndex(id);
	int value = getValueID(id);

	isAdd ? value ++ : value--;
	variables[bucket] = value;
}

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}

int main() {
	for (int i = 0; i < 52; i++) {
		variables[i] = 0;
	}

	return yyparse();
}