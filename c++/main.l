%{
#include "parser.hpp"

void yyerror(const char *s);
%}

%option yylineno

id				[a-zA-Z_][a-zA-Z0-9_]*
number			[0-9]+
blank			[ \t\n]
string			["].*["]
charaster		['].*[']
comment_str		\/\/.*
comment_strs	\/\*(.*\n)*.*\*\/
%%

"print"							{ return PRINT; }

{id}							{ yylval.id = yytext[0]; return ID; }
{number}						{ yylval.num = atoi(yytext); return NUM; }
{string}						{ yylval.str = yytext; return STRING; }

"++"							{ return INCREMENT; }
"--"							{ return DECREMENT; }
"+="							{ return ADDASSIGMENT; }
"-="							{ return SUBTRACTASSIGMENT; }

{comment_str}					{ ; }
{comment_strs}					{ ; }
{blank}							{ ; }
[-+*/=();]						{ return *yytext; }
.								{ yyerror("Invalid character"); }

%%

int yywrap(void) {
	return 1;
}
