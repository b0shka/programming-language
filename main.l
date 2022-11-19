%{
#include "parser.h"

void yyerror(char *s);
%}

%option yylineno

number			[0-9]+
time			..:..
filename		[.+]?[.+]?[\/]?[a-zA-Z_][a-zA-Z0-9_\/.-]+
name			[a-zA-Z_][a-zA-Z0-9_]*
comment_str		\/\/.*
comment_strs	\/\*(.*\n)*.*\*\/

%%

PATH_FILE						{ return PATH; }
add_device						{ return ADD; }
device|DEVICE					{ return DEVICE; }
time|TIME						{ return TIME; }
if								{ return IF; }
else							{ return ELSE; }
on								{ yylval.job_status = true; return ACTION_JOB; }
off								{ yylval.job_status = false; return ACTION_JOB; }

{time}							{ yylval.time = strdup(yytext); return TIME_VALUE; }
{name}							{ yylval.name = strdup(yytext); return NAME; }
{filename}						{ yylval.filename = strdup(yytext); return FILENAME; }

\"								{ return QUOTE; }

{comment_str}					{ ; }
{comment_strs}					{ ; }
[ \t\n]							{ ; }

[(){}=]							{ return *yytext; }
.								{ yyerror("Invalid character"); }

%%

int yywrap(void) {
	return 1;
}
