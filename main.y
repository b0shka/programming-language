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

%token PATH ADD DEVICE END
%token TIME QUOTE IF ELSE ARROW
%token<name> NAME
%token<filename> FILENAME
%token<time> TIME_VALUE
%token<num> NUMBER
%token<action> ACTION

%type COMMANDS COMMAND OP
%type<event> ACTIONS
%type<name> VAL

%%

COMMANDS:
|			COMMANDS COMMAND
|			COMMANDS OP
;

COMMAND:	PATH '=' QUOTE FILENAME QUOTE				{ G_PATH_FILE_OUTPUT = strdup($4); }
|			ACTIONS										{ 
															if (check_action_device($1->name, $1->action)) {
																if ($1->time != NULL)
																	add_event($1);
																else 
																	processing_actions($1);
															}
															else
																yyerror("This action is not supported");
														}
|			END											{ monitoring(); }
;

OP:			IF VAL ARROW ACTIONS						{ 
															add_condition($2, $4);
															if (checking_condition($2))
																processing_actions($4);
 														}
;

ACTIONS:	DEVICE VAL									{ 
															if (get_index_device($2) != -1) {
																$$->name = $2;
																$$->time = NULL;
																$$->status_complete = false;
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

%%
