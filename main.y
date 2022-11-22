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
	struct Event *events;
}

%token PATH ADD DEVICE END
%token TIME QUOTE IF ELSE
%token<name> NAME
%token<filename> FILENAME
%token<time> TIME_VALUE
%token<num> NUMBER
%token<action> ACTION_VALUE

%type COMMANDS COMMAND
%type<events> OPS OP
%type<event> ACTION
%type<name> VAL

%%

COMMANDS:
|			COMMANDS COMMAND
|			COMMANDS OPS
;

COMMAND:	PATH '=' QUOTE FILENAME QUOTE				{ G_PATH_FILE_OUTPUT = strdup($4); }
|			ACTION										{ 
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

OPS:		IF VAL '{' OP '}'							{ 
															add_condition($2); 
															checking_condition($2);
														}
;

OP:			ACTION										{ add_event_condition($1); }
|			OP ACTION									{ add_event_condition($2); }
;

ACTION:		DEVICE VAL									{ 
															if (get_index_device($2) != -1) {
																$$->name = $2;
																$$->time = NULL;
																$$->status_complete = false;
															}
															else
																yyerror("Device not found");
														}
|			ACTION ACTION_VALUE							{ $$->action = strdup($2); }
//|			ACTION NUMBER								{ $$->target = $2; }
|			ACTION TIME TIME_VALUE						{ $$->time = strdup($3); }
;

VAL:		'(' QUOTE NAME QUOTE ')'					{ $$ = strdup($3); }
|			QUOTE NAME QUOTE							{ $$ = strdup($2); }
;

%%
