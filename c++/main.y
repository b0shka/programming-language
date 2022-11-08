%{
#include "main.h"
#include <map>
#include <string>

extern int yylineno;
extern int yylex();

void yyerror (const char *s);

map <char, int> vars;
%}

%union {
	char id;
	int num;
	char* str;
}

%token PRINT INCREMENT DECREMENT ADDASSIGMENT SUBTRACTASSIGMENT
%token <id> ID
%token <num> NUM
%token <str> STRING
%type <num> EXPR EXPR1 TERM VAL

%%

PROGRAM:	PRINT '(' EXPR ')' ';'				{ cout << $3 << endl; }
|			PROGRAM PRINT '(' EXPR ')' ';'		{ cout << $4 << endl; }

//|			PRINT '(' STRING ')' ';'			{ printf("%s\n", $3); }
//|			PROGRAM PRINT '(' STRING ')' ';'	{ printf("%s\n", $4); }	

|			EXPR ';'							{ ; }
|			PROGRAM EXPR ';'					{ ; }
//|			STRING ';'							{ ; }
//|			PROGRAM STRING ';'					{ ; }
;

EXPR:		EXPR1
|			ID '=' EXPR1						{ $$ = $3; vars[$1] = $3; }
//|			ID ADDASSIGMENT EXPR1				{ assigmentUnaryValueID($1, $3, true); }
//|			ID SUBTRACTASSIGMENT EXPR1			{ assigmentUnaryValueID($1, $3, false); }
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

VAL:		'(' EXPR ')'						{ $$ = $2; }
|			'-' VAL								{ $$ = -$2; }
|			NUM									{ $$ = $1; }
|			ID									{
													if (vars[$1])
														$$ = vars[$1];
													else
														yyerror("Undefined variable");
												}
;

%%

void yyerror(const char *s) {
	cout << s << ", line " << yylineno << endl;
	exit(1);
}
