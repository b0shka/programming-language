%{
#include "y.tab.h"
#include <stdio.h>

void yyerror(char *s);
%}

%option yylineno

alpha 			[a-zA-Z]
digit			[0-9]
number			{digit}+
float_number	{digit}+\.{digit}{1,6}
string			["].*["]
charaster		['].*[']
unary			"++" | "--"

%%

"print"							{ return PRINT; }

{number}						{ yylval.num = atoi(yytext); return NUMBER; }
{float_number}					{ yylval.float_num = atof(yytext); return FLOAT_NUM; }
{string}						{ yylval.string = yytext; printf("%s\n", yytext) ;return STRING; }
{charaster}						{ yylval.charaster = yytext[1]; return CHARACTER; }

"+"								{ return ADD; }
"-"								{ return SUBTRACT; }
"*"								{ return MULTIPLY; }
"/"								{ return DIVIDE; }

\/\/.*							{ ; }
\/\*(.*\n)*.*\*\/				{ ; }
[ \t\n]*						{ ; }
.								{ return *yytext; }

%%

int yywrap(void) {
	return 1;
}