%{
#include "main.h"
%}

%code requires {
	#include <stdbool.h>
}

%union {
	char *action;
	char *time;
	char *name;
	char *filename;
	int num;
	struct Event *event;
}

%token PATH ADD DEVICE END_STR
%token TIME QUOTE IF ELSE 
%token<name> NAME
%token<filename> FILENAME
%token<time> TIME_VALUE
%token<num> NUMBER
%token<action> ACTION

%type COMMANDS COMMAND OP END
%type<event> ACTIONS
%type<name> VAL

%%

COMMANDS:
|			COMMANDS COMMAND
|			COMMANDS OP
;

COMMAND:	PATH '=' QUOTE FILENAME QUOTE				{ G_PATH_FILE_OUTPUT = $4; }
|			ADD VAL										{ add_device($2); }
|			ACTIONS										{ processing_actions($1); }
|			END
;

OP:			IF VAL '{' ACTIONS '}'						{ 
															if (checking_condition($2))
																processing_actions($4);
 														}
;

ACTIONS:	DEVICE VAL									{ 
															if (get_index_device($2) != -1) {
																$$->name = $2;
																$$->time = NULL;
															}
															else
																yyerror("Device not found");
														}
|			ACTIONS ACTION								{ $$->action = strdup($2); }
//|			ACTIONS NUMBER								{ $$->target = $2; }
|			ACTIONS TIME TIME_VALUE						{ $$->time = strdup($3); }
;

VAL:		'(' QUOTE NAME QUOTE ')'					{ $$ = strdup($3); }
|			QUOTE NAME QUOTE							{ $$ = strdup($2); }
;

END:		END_STR										{ monitoring(); }

%%
