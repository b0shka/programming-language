%{
#include "main.h"
%}

%code requires {
	#include <stdbool.h>
}

%union {
	bool job_status;
	char* name;
}

%token ADD_DEVICE ADD_SENSOR GET_DEVICE GET_SENSOR DEVICE SENSOR QUOTE 
%token<name> NAME
%token<job_status> ACTION_JOB

%type COMMANDS COMMAND ADD ACTIONS
%type GET
%type<name> VAL

%%

COMMANDS:
|			COMMANDS COMMAND ';'
;

COMMAND:	ADD
|			GET
|			ACTIONS
;

ADD:		ADD_DEVICE VAL						{ add_device(&$2); }
|			ADD_SENSOR VAL						{ add_sensor(&$2); }
;

GET:		GET_DEVICE VAL						{ get_device(&$2); }
|			GET_SENSOR VAL						{ get_sensor(&$2); }
;

ACTIONS:	DEVICE VAL ACTION_JOB				{
													if (check_device(&$2)) {
														if ($3)
															turn_on_device(&$2);
														else
															turn_off_device(&$2);
													}
													else
														yyerror("Device not found");
												}
|			SENSOR VAL ACTION_JOB				{
													if (check_sensor(&$2)) {
														if ($3)
															turn_on_sensor(&$2);
														else
															turn_off_sensor(&$2);
													}
													else
														yyerror("Sensor not found");
												}
;

VAL:		'(' QUOTE NAME QUOTE ')'			{ $$ = $3; }
|			QUOTE NAME QUOTE					{ $$ = $2; }
;

%%
