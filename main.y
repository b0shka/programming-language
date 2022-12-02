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
	char *path;
	char *num;
	struct Expression *expr;
	struct Event *event;
	struct Event *events;
}

%token FILE_OUTPUT ADD DEVICE END TIME QUOTE IF AND
%token<name> NAME
%token<path> PATH
%token<time> TIME_VALUE
%token<num> NUMBER
%token<action> ACTION_VALUE

%type COMMANDS COMMAND OP
%type<expr> OPS
%type<event> ACTION CONDITION
%type<name> VAL

%%

COMMANDS:
|			COMMANDS COMMAND
|			COMMANDS OPS
;

COMMAND:	FILE_OUTPUT '=' QUOTE PATH QUOTE	{ G_PATH_FILE_OUTPUT = strdup($4); }
|			CONDITION							{ 
													if (check_action_device($1->name, $1->action)) {
														if ($1->time != NULL)
															add_event($1);
														else 
															processing_actions($1);
													}
													else
														yyerror("This action is not supported");
												}
|			END									{ monitoring(); }
;

OPS:		IF 									{}
|			OPS VAL								{
													struct Expression *expression = (struct Expression*)malloc(sizeof(struct Expression));
													expression->name = $2;
													$$ = expression;
												}
|			OPS AND								{}
|			OPS TIME '>' TIME_VALUE				{ expression->time_start = $4; }
|			OPS TIME '<' TIME_VALUE				{ expression->time_end = $4; }
|			OPS '{' OP '}'						{ add_condition($$); }
;

OP:			CONDITION							{ add_event_condition($1); }
|			OP CONDITION						{ add_event_condition($2); }
;

CONDITION:	ACTION								{ $$ = $1; }
|			ACTION NUMBER						{ $$->target = $2; }
|			ACTION VAL							{ $$->notification = $2; }
|			CONDITION TIME TIME_VALUE			{ $$->time = $3; }
;

ACTION:		DEVICE VAL ACTION_VALUE				{
													struct Event *event = (struct Event*)malloc(sizeof(struct Event));
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

VAL:		'(' QUOTE NAME QUOTE ')'			{ $$ = strdup($3); }
|			QUOTE NAME QUOTE					{ $$ = strdup($2); }
;

%%
