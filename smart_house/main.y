%{
#include "main.h"
%}

%code requires {
	#include <stdbool.h>
}

%union {
	bool job_status;
	char *name;
	char *filename;
	struct Device *device;
	struct Sensor *sensor;
	struct Variable *var;
}

%token PATH_FILE ADD_DEVICE ADD_SENSOR GET_DEVICE GET_SENSOR DEVICE SENSOR QUOTE ARG IF ELSE JOB_STATUS 
%token<name> NAME
%token<filename> FILENAME
%token<job_status> ACTION_JOB

%type COMMANDS COMMAND PATH ADD GET ACTIONS
%type<device> GET_DEV
%type<sensor> GET_SEN
%type<var> OP
%type<name> VAL

%%

COMMANDS:
|			COMMANDS COMMAND
;

COMMAND:	PATH
|			ADD
|			GET
|			OP
|			ACTIONS
;

PATH:		PATH_FILE '=' QUOTE FILENAME QUOTE	{ G_PATH_FILE_DATA = $4; }
;

ADD:		ADD_DEVICE VAL						{ add_device(&$2); }
|			ADD_SENSOR VAL						{ add_sensor(&$2); }
;

GET:		GET_DEV
|			GET_SEN
|			NAME '=' GET_DEV					{ add_var_device(&$1, $3); }
;

GET_DEV:	GET_DEVICE VAL						{ $$ = get_device(&$2); }
;

GET_SEN:	GET_SENSOR VAL						{ $$ = get_sensor(&$2); }
;

OP:			'{' ACTIONS '}'						{  }
|	IF '(' NAME ARG JOB_STATUS ')' OP ELSE OP	{ 
													$$ = get_var_by_name(&$3);
													if ($$->device->job_status) {
														// actions
													}
													else {
														// actions
													}
												}
;

ACTIONS:	DEVICE VAL ACTION_JOB				{
													if (check_device(&$2)) {
														if ($3)
															turn_on_device(&$2);
														else
															turn_off_device(&$2);

														overwriting_data_file();
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

														overwriting_data_file();
													}
													else
														yyerror("Sensor not found");
												}
;

VAL:		'(' QUOTE NAME QUOTE ')'			{ $$ = $3; }
|			QUOTE NAME QUOTE					{ $$ = $2; }
;

%%
