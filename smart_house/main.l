%{
#include "parser.h"

void yyerror(char *s);
%}

%option yylineno

name			[a-zA-Z_][a-zA-Z0-9_]*
comment_str		\/\/.*
comment_strs	\/\*(.*\n)*.*\*\/

%%

add_device						{ return ADD_DEVICE; }
add_sensor						{ return ADD_SENSOR; }

device							{ return DEVICE; }
sensor							{ return SENSOR; }
on								{ yylval.job_status = true; return ACTION_JOB; }
off								{ yylval.job_status = false; return ACTION_JOB; }
{name}							{ yylval.name = strdup(yytext); return NAME; }
\"								{ return QUOTE; }

{comment_str}					{ ; }
{comment_strs}					{ ; }
[ \t\n]							{ ; }
[();]							{ return *yytext; }
.								{ yyerror("Invalid character"); }

%%

int yywrap(void) {
	return 1;
}
