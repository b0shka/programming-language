%{
#include "main.h"
%}

%code requires {
	#include <stdbool.h>
}

%union {
	bool job_status;
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
%token<job_status> ACTION_JOB

%type COMMANDS COMMAND END
%type<event> ACTIONS
%type<name> VAL

%%

COMMANDS:
|			COMMANDS COMMAND
;

COMMAND:	PATH '=' QUOTE FILENAME QUOTE				{ G_PATH_FILE_DATA = $4; }
|			ADD VAL										{ add_device(strdup($2)); }
|			ACTIONS										{
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
|			END
;

ACTIONS:	DEVICE VAL									{ 
															if (check_device($2)) {
																$$->name = strdup($2);
																$$->time = NULL;
															}
															else
																yyerror("Device not found");
														}
|			ACTIONS ACTION_JOB							{ 
															if ($2)
																$$->action = "on";
															else
																$$->action = "off";
 														}
|			ACTIONS TIME TIME_VALUE						{ $$->time = strdup($3); }
;

VAL:		'(' QUOTE NAME QUOTE ')'					{ $$ = $3; }
|			QUOTE NAME QUOTE							{ $$ = $2; }
;

END:		END_STR										{ monitoring(); }

%%
