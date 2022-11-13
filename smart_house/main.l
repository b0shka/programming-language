%{
#include "parser.h"

void yyerror(char *s);
%}

%option yylineno

filename		[.+]?[.+]?[\/]?[a-zA-Z_][a-zA-Z0-9_\/.-]+
name			[a-zA-Z_][a-zA-Z0-9_]*
comment_str		\/\/.*
comment_strs	\/\*(.*\n)*.*\*\/

%%

PATH_FILE						{ return PATH_FILE; }

add_device						{ return ADD_DEVICE; }
add_sensor						{ return ADD_SENSOR; }
get_device						{ return GET_DEVICE; }
get_sensor						{ return GET_SENSOR; }

device|DEVICE					{ return DEVICE; }
sensor|SENSOR					{ return SENSOR; }
on								{ yylval.job_status = true; return ACTION_JOB; }
off								{ yylval.job_status = false; return ACTION_JOB; }
{name}|{filename}				{ yylval.name = strdup(yytext); return NAME; }
\"								{ return QUOTE; }

{comment_str}					{ ; }
{comment_strs}					{ ; }
[ \t\n]							{ ; }
[()=;]							{ return *yytext; }
.								{ yyerror("Invalid character"); }

%%

int yywrap(void) {
	return 1;
}
