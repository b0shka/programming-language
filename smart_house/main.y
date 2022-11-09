%{
#include "main.h"

extern int yylineno;
extern int yylex();

void yyerror (char *s);
%}

%code requires {
	#include <stdbool.h>
}

%union {
	struct {
		bool work;
	} actions;
	struct {
		int charge;
	} state;
	char* device;
}

%token<device> TEAPOT SENSOR TEMPERATURE
%token<actions> ACTION
%token CHARGE STATISTIC
%type COMMANDS COMMAND

%%

COMMANDS:
|			COMMANDS COMMAND ';'
;

COMMAND:	TEAPOT ACTION						{
													if ($2.work)
														printf("[TEAPOT] on\n");
													else
														printf("[TEAPOT] off\n");
												}
|			TEAPOT STATISTIC					{ printf("[TEAPOT] statistic\n"); }
|			TEAPOT CHARGE						{ printf("[TEAPOT] charge\n"); }

|			SENSOR TEMPERATURE ACTION			{
													if ($3.work)
														printf("[SENSOR TEMPERATURE] on\n");
													else
														printf("[SENSOR TEMPERATURE] off\n"); 
												}
|			SENSOR TEMPERATURE STATISTIC		{ printf("[SENSOR TEMPERATURE] statistic\n"); }
|			SENSOR TEMPERATURE CHARGE			{ printf("[SENSOR TEMPERATURE] charge\n"); }
;

%%

void yyerror(char *s) {
	fprintf(stderr, "%s, line %d\n", s, yylineno);
	exit(1);
}
