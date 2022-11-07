%{
#include "y.tab.h"

void yyerror(char *s);
int countn = 0;
%}

%option yylineno

alpha 			[a-zA-Z]
digit			[0-9]
id				[a-zA-Z][a-zA-Z0-9]*
number			{digit}+
float_number	{digit}+\.{digit}{1,6}
string			["].*["]
charaster		['].*[']
unary			"++" | "--"
comment_str		\/\/.*
comment_strs	\/\*(.*\n)*.*\*\/

%%

"print"							{ return PRINT; }

{id}							{ yylval.id = yytext[0]; return ID; }
{number}						{ yylval.num = atoi(yytext); return NUMBER; }
{float_number}					{ yylval.float_num = atof(yytext); return FLOAT_NUM; }
{string}						{ yylval.string = yytext ;return STRING; }
{charaster}						{ yylval.charaster = yytext[1]; return CHARACTER; }

"+"								{ return ADD; }
"-"								{ return SUBTRACT; }
"*"								{ return MULTIPLY; }
"/"								{ return DIVIDE; }
"++"							{ return INCREMENT; }
"--"							{ return DECREMENT; }
"+="							{ return ADDASSIGMENT; }
"-="							{ return SUBTRACTASSIGMENT; }

{comment_str}					{ ; }
{comment_strs}					{ ; }
[ \t]*							{ ; }
[\n]							{ countn++; }
[=();]							{ return *yytext; }
.								{ ECHO; yyerror("unexpected charecter"); }

%%

int yywrap(void) {
	return 1;
}