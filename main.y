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
	struct Event *event;
}

%token PATH ADD DEVICE END_STR
%token TIME QUOTE IF ELSE 
%token<name> NAME
%token<filename> FILENAME
%token<time> TIME_VALUE
%token<action> ACTION

%type COMMANDS COMMAND END
%type<event> ACTIONS
%type<name> VAL

%%

COMMANDS:
|			COMMANDS COMMAND
;

COMMAND:	PATH '=' QUOTE FILENAME QUOTE				{ G_PATH_FILE_DATA = $4; }
|			ADD VAL										{ 
															if (get_index_aviable_device(strdup($2)) != -1)
																add_device($2);
															else
																yyerror("Such a device is not supported");
														}
|			ACTIONS										{
															if (check_action_on_aviable($1->name, $1->action)) {
																if ($1->time != NULL)
																	add_event($1);
																else {
																	if (strcmp($1->action, "on") == 0)
																		turn_on_device($1->name);

																	else if (strcmp($1->action, "off") == 0)
																		turn_off_device($1->name);

																	overwriting_data_file();
																}
															}
															else
																yyerror("This action is not supported");
														}
|			END
;

ACTIONS:	DEVICE VAL									{ 
															if (get_index_device(strdup($2)) != -1) {
																$$->name = strdup($2);
																$$->time = NULL;
															}
															else
																yyerror("Device not found");
														}
|			ACTIONS ACTION								{ $$->action = strdup($2); }
|			ACTIONS TIME TIME_VALUE						{ $$->time = strdup($3); }
;

VAL:		'(' QUOTE NAME QUOTE ')'					{ $$ = $3; }
|			QUOTE NAME QUOTE							{ $$ = $2; }
;

END:		END_STR										{ monitoring(); }

%%
