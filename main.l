%{
#include "parser.h"

void yyerror(char *s);
%}

%option yylineno

number			[0-9]+
time			([0-1]?[0-9]|2[0-3]):[0-5][0-9]
filename		[.+]?[.+]?[\/]?[a-zA-Z_][a-zA-Z0-9_\/.-]+
name			[a-zA-Z_][a-zA-Z0-9_]*
comment_str		\/\/.*
comment_strs	\/\*(.*\n)*.*\*\/
actions			on|off|open|target|vacuum|boil

%%

FILE_OUTPUT						{ return PATH; }
device|DEVICE					{ return DEVICE; }
time|TIME						{ return TIME; }
if								{ return IF; }
else							{ return ELSE; }

{actions}						{ yylval.action = strdup(yytext); return ACTION; }
{time}							{ yylval.time = strdup(yytext); return TIME_VALUE; }
{number}						{ yylval.num = atoi(yytext); return NUMBER; }
{name}							{ yylval.name = strdup(yytext); return NAME; }
{filename}						{ yylval.filename = strdup(yytext); return FILENAME; }

\"								{ return QUOTE; }
->								{ return ARROW; }

{comment_str}					{ ; }
{comment_strs}					{ ; }
[ \t\n]							{ ; }

[()=]							{ return *yytext; }
<<EOF>>							{ return END; }
.								{ yyerror("Invalid character"); }

%%

int yywrap(void) {
	return 1;
}
