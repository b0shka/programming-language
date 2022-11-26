%{
#include "main.h"
%}

%union {
	char *action;
	char *time;
	char *name;
	char *path;
	char *num;
	struct Event *event;
	struct Event *events;
}

%token FILE_OUTPUT ADD DEVICE END TIME QUOTE IF
%token<name> NAME
%token<path> PATH
%token<time> TIME_VALUE
%token<num> NUMBER
%token<action> ACTION_VALUE

%type COMMANDS COMMAND
%type<events> OPS OP
%type<event> ACTION CONDITION
%type<name> VAL

%%

COMMANDS:
|			COMMANDS COMMAND
|			COMMANDS OPS
;

COMMAND:	FILE_OUTPUT '=' QUOTE PATH QUOTE			{ G_PATH_FILE_OUTPUT = strdup($4); }
|			CONDITION									{ 
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

OP:			CONDITION									{ add_event_condition($1); }
|			OP CONDITION								{ add_event_condition($2); }
;

CONDITION:	ACTION										{ $$ = $1; }
|			ACTION NUMBER								{ $$->target = $2; }
|			CONDITION TIME TIME_VALUE					{ $$->time = $3; }
;

ACTION:		DEVICE VAL ACTION_VALUE						{
															struct Event *event = (struct Event*)malloc(sizeof(struct Event));;
															if (get_index_device($2) != -1) {
																event->name = $2;
																event->action = strdup($3);
																event->time = NULL;
																event->status_complete = false;
																$$ = event;
															}
															else
																yyerror("Device not found");
														}
;

VAL:		'(' QUOTE NAME QUOTE ')'					{ $$ = strdup($3); }
|			QUOTE NAME QUOTE							{ $$ = strdup($2); }
;

%%
