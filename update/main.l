%{

//typedef struct {
//	char* str;
//	char* id;
//	int num;
//} TYPES;
//#define YYSTYPE TYPES
//#define YYSTYPE char*
#include "y.tab.h"

void yyerror(char *s);
%}

%option yylineno

id				[a-zA-Z_][a-zA-Z0-9_]*
number			[0-9]+
float_number	[0-9]+\.[0-9]{1,6}
string			["].*["]
charaster		['].*[']
comment_str		\/\/.*
comment_strs	\/\*(.*\n)*.*\*\/

%%

"print"							{ return PRINT; }

{id}							{ yylval.id = yytext; return ID; }
{number}						{ yylval.num = atoi(yytext); return NUM; }

{comment_str}					{ ; }
{comment_strs}					{ ; }
[ \t\r\n]						{ ; }
[-+*/=();]						{ return *yytext; }
.								{ yyerror("Invalid character"); }

%%

int yywrap(void) {
	return 1;
}