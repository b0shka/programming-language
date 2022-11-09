%{
#include "parser.h"

void yyerror(char *s);
%}

%option yylineno

id				[a-zA-Z_][a-zA-Z0-9_]*
number			[0-9]+
comment_str		\/\/.*
comment_strs	\/\*(.*\n)*.*\*\/

%%

teapot							{ return TEAPOT; }
sensor							{ return SENSOR; }
temperature						{ return TEMPERATURE; }
on								{ yylval.actions.work = true; return ACTION; }
off								{ yylval.actions.work = false; return ACTION; }
charge							{ return CHARGE; }
statistic						{ return STATISTIC; }

{comment_str}					{ ; }
{comment_strs}					{ ; }
[ \t\n]							{ ; }
[;]								{ return *yytext; }
.								{ yyerror("Invalid character"); }

%%

int yywrap(void) {
	return 1;
}
