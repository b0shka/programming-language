%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern int yylex();
extern int yywrap();
void yyerror (char *s);
%}

%union {
	int num;
	float float_num;
	char *string;
	char charaster;
}

%token PRINT ID ADD SUBTRACT MULTIPLY DIVIDE
%token <num> NUMBER
%token <float_num> FLOAT_NUM
%token <string> STRING
%token <charaster> CHARACTER
%type <num> exp term
%left '+' '-'
%left '*' '/'

%%

program	:	PRINT '(' exp ')' ';'				{ printf("%d\n", $3); }
		|	program PRINT '(' exp ')' ';'		{ printf("%d\n", $4); }

		|	PRINT '(' STRING ')' ';'			{ printf("%s\n", $3); }
		|	program PRINT '(' STRING ')' ';'	{ printf("%s\n", $4); }

		|	PRINT '(' CHARACTER ')' ';'			{ printf("%c\n", $3); }
		|	program PRINT '(' CHARACTER ')' ';'	{ printf("%c\n", $4); }

		|	exp ';'								{  }
		|	program exp ';'						{  }
		|	STRING ';'							{  }
		|	program STRING ';'					{  }
		|	CHARACTER ';'						{  }
		|	program CHARACTER ';'				{  }

		;

exp		: term									{ $$ = $1; }
		| exp ADD term							{ $$ = $1 + $3; }
		| exp SUBTRACT term						{ $$ = $1 - $3; }
		//| exp MULTIPLY term						{ $$ = $1 * $3; }
		//| exp DIVIDE term						{ $$ = $1 / $3; }
		;

term	: '(' exp ')'							{ $$ = $2; }
		| SUBTRACT term							{ $$ = -$2; }
		| NUMBER								{ $$ = $1; }
		;

%%

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}

int main() {
	return yyparse();
}