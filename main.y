%{
#include "main.h"
%}

%code requires {
	#include <stdbool.h>

	struct Argument {
		char *type;
		char *value;
	};
}

%union {
	char *action;
	char *time;
	char *name;
	char *path;
	char *num;
	struct Argument arg;
	struct Arguments *args;
	struct Event *event;
}

%token FILE_OUTPUT ADD DEVICE END TIME QUOTE IF AND
%token<name> NAME
%token<path> PATH
%token<time> TIME_VALUE
%token<num> NUMBER
%token<action> ACTION_VALUE

%type COMMANDS COMMAND OP OPS
%type<arg> ARG
%type<args> ARGS
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

OPS:		IF ARGS '{' OP '}'					{
													if ($2->name == NULL)
														yyerror("Incorrect condition argument");
													add_condition($2);
													free($2);
												}
;

OP:			CONDITION							{ add_event_condition($1); }
|			OP CONDITION						{ add_event_condition($2); }
;

ARGS:											{}
|			ARG									{
													$$ = (struct Arguments*)malloc(sizeof(struct Arguments));
													$$->name = NULL;
													$$->time_start = NULL;
													$$->time_end = NULL;

													if (strcmp($1.type, "name") == 0)
														$$->name = strdup($1.value);
													else if (strcmp($1.type, "time_start") == 0)
														$$->time_start = strdup($1.value);
													else if (strcmp($1.type, "time_end") == 0)
														$$->time_end = strdup($1.value);
												}
|			ARGS AND ARG						{
													if (strcmp($3.type, "name") == 0) {
														if ($$->name != NULL)
															yyerror("Incorrect condition argument");
														$$->name = strdup($3.value);
													}
													else if (strcmp($3.type, "time_start") == 0) {
														if ($$->time_start != NULL)
															yyerror("Incorrect condition argument");
														$$->time_start = strdup($3.value);
													}
													else if (strcmp($3.type, "time_end") == 0) {
														if ($$->time_end != NULL)
															yyerror("Incorrect condition argument");
														$$->time_end = strdup($3.value);
													}
												}
;

ARG:		VAL									{
													struct Argument *argument = (struct Argument*)malloc(sizeof(struct Argument));
													argument->type = "name";
													argument->value = $1;
													$$ = *argument;
													free(argument);
												}
|			TIME '>' TIME_VALUE					{
													struct Argument *argument = (struct Argument*)malloc(sizeof(struct Argument));
													argument->type = "time_start";
													argument->value = $3;
													$$ = *argument;
													free(argument);
												}
|			TIME '<' TIME_VALUE					{
													struct Argument *argument = (struct Argument*)malloc(sizeof(struct Argument));
													argument->type = "time_end";
													argument->value = $3;
													$$ = *argument;
													free(argument);
												}
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
