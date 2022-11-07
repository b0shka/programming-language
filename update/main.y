%{
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

extern int yylineno;
extern int yylex();
void yyerror (char *s);

//typedef struct {
//	char* str;
//	char* id;
//	int num;
//} TYPES;
//#define YYSTYPE TYPES
//#define YYSTYPE char*

//int variables[52];
//int newValueID(char *id);
//int getValueID(char *id);
//void assigmentUnaryValueID(char *id, int val, bool isAdd);
//void unaryID(char *id, bool isAdd);

//%token INCREMENT DECREMENT ADDASSIGMENT SUBTRACTASSIGMENT
%}

%union {
	char* str;
	char* id;
	int num;
}

%token PRINT
%token <id> ID
%token <num> NUM

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
|			ID '=' EXPR							{ ; }
//|			ID ADDASSIGMENT ASSIGNMENT			{ assigmentUnaryValueID($1, $3, true); }
//|			ID SUBTRACTASSIGMENT ASSIGNMENT		{ assigmentUnaryValueID($1, $3, false); }
//|			ID INCREMENT						{ unaryID($1, true); }
//|			ID DECREMENT						{ unaryID($1, false); }
;

EXPR1:		TERM
|			EXPR1 '+' TERM						{ $$ = $1 + $3; }
|			EXPR1 '-' TERM						{ $$ = $1 - $3; }
;

TERM:		VAL
|			TERM '*' VAL						{ $$ = $1 * $3; }
|			TERM '/' VAL						{ $$ = $1 / $3; }
;

VAL:		'(' EXPR1 ')'						{ $$ = $2; }
|			'-' VAL								{ $$ = -$2; }
|			NUM									{ $$ = $1; }
|			ID									{ ; }
;

%%

//int newValueID(char *id) {

//}

//int getValueID(char *id) {
//	int bucket = computeIDIndex(id);
//	return variables[bucket];
//}

//void assigmentUnaryValueID(char id, int val, bool isAdd) {
//	int bucket = computeIDIndex(id);
//	int value = getValueID(id);
//	variables[bucket] = isAdd ? value + val : value - val;
//}

//void unaryID(char id, bool isAdd) {
//	int bucket = computeIDIndex(id);
//	int value = getValueID(id);

//	isAdd ? value ++ : value--;
//	variables[bucket] = value;
//}

void yyerror(char *s) {
	fprintf(stderr, "%s, line %d\n", s, yylineno);
	exit(1);
}

int main() {
	return yyparse();
}