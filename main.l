%{
#include "parser.h"

void yyerror(char *s);
%}

%option yylineno

number			[0-9]+
time			([0-1]?[0-9]|2[0-3]):[0-5][0-9]
path			[.+]?[.+]?[\/]?[a-zA-Z_][a-zA-Z0-9_\/.-]+
name			[a-zA-Z_][a-zA-Z0-9_]*
comment_str		\/\/.*
comment_strs	\/\*(.*\n)*.*\*\/
actions			on|off|open|vacuum|boil|target|sos|host_notification

%%

FILE_OUTPUT						{ return FILE_OUTPUT; }
device|DEVICE					{ return DEVICE; }
time|TIME						{ return TIME; }
if								{ return IF; }
and								{ return AND; }

{actions}						{ yylval.action = strdup(yytext); return ACTION_VALUE; }
{time}							{ yylval.time = strdup(yytext); return TIME_VALUE; }
{number}						{ yylval.num = strdup(yytext); return NUMBER; }
{name}							{ yylval.name = strdup(yytext); return NAME; }
{path}							{ yylval.path = strdup(yytext); return PATH; }

\"								{ return QUOTE; }

{comment_str}					{ ; }
{comment_strs}					{ ; }
[ \t\n]							{ ; }

[(){}=<>]						{ return *yytext; }
<<EOF>>							{ return END; }
.								{ yyerror("Invalid character"); }

%%

int yywrap(void) {
	return 1;
}
