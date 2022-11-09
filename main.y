%{
#include "main.h"

extern int yylineno;
extern int yylex();

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
	char* str;
}

%token PRINT INCREMENT DECREMENT ADDASSIGMENT SUBTRACTASSIGMENT
%token <id> ID
%token <num> NUM
%token <string> STRING
%type <num> EXPR EXPR1 TERM VAL

%%

PROGRAM:	PRINT '(' EXPR ')' ';'				{ printf("%d\n", $3); }
|			PROGRAM PRINT '(' EXPR ')' ';'		{ printf("%d\n", $4); }

//|			PRINT '(' STRING ')' ';'			{ printf("%s\n", $3); }
//|			PROGRAM PRINT '(' STRING ')' ';'	{ printf("%s\n", $4); }	

|			EXPR ';'							{ ; }
|			PROGRAM EXPR ';'					{ ; }
//|			STRING ';'							{ ; }
//|			PROGRAM STRING ';'					{ ; }
;

EXPR:		EXPR1
|			ID '=' EXPR1						{ assigmentValueID($1, $3); }
|			ID ADDASSIGMENT EXPR1				{ assigmentUnaryValueID($1, $3, true); }
|			ID SUBTRACTASSIGMENT EXPR1			{ assigmentUnaryValueID($1, $3, false); }
|			ID INCREMENT						{ unaryID($1, true); }
|			ID DECREMENT						{ unaryID($1, false); }
;

EXPR1:		TERM
|			EXPR1 '+' TERM						{ $$ = $1 + $3; }
|			EXPR1 '-' TERM						{ $$ = $1 - $3; }
;

TERM:		VAL
|			TERM '*' VAL						{ $$ = $1 * $3; }
|			TERM '/' VAL						{ $$ = $1 / $3; }
;

VAL:		'(' EXPR ')'						{ $$ = $2; }
|			'-' VAL								{ $$ = -$2; }
|			NUM									{ $$ = $1; }
|			ID									{ $$ = getValueID($1); }
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
	fprintf(stderr, "%s, line %d\n", s, yylineno);
	exit(1);
}
