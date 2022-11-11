%{
#include "main.h"

extern int yylineno;
extern int yylex();

void yyerror (char *s);

int count_devices = 0;
int count_sensors = 0;
char **sensors = NULL;
char **devices = NULL;

void add_device(char* name);
void add_sensor(char* name);
bool check_device(char* name);
bool check_sensor(char* name);
%}

%code requires {
	#include <stdbool.h>
}

%union {
	bool job_status;
	char* name;
}

%token ADD_DEVICE ADD_SENSOR DEVICE SENSOR QUOTE 
%token<name> NAME
%token<job_status> ACTION_JOB

%type COMMANDS COMMAND ADD ACTIONS
%type<name> VAL

%%

COMMANDS:
|			COMMANDS COMMAND ';'
;

COMMAND:	ADD
|			ACTIONS
;

ADD:		ADD_DEVICE VAL						{ add_device($2); }
|			ADD_SENSOR VAL						{ add_sensor($2); }
;

ACTIONS:	DEVICE VAL ACTION_JOB				{
													if (check_device($2)) {
														if ($3)
															printf("[DEVICE %s] on\n", $2);
														else
															printf("[DEVICE %s] off\n", $2);
													}
													else
														yyerror("Device not found");
												}
|			SENSOR VAL ACTION_JOB				{
													if (check_sensor($2)) {
														if ($3)
															printf("[SENSOR %s] on\n", $2);
														else
															printf("[SENSOR %s] off\n", $2); 
													}
													else
														yyerror("Sensor not found");
												}
;

VAL:		'(' QUOTE NAME QUOTE ')'			{ $$ = $3; }
|			QUOTE NAME QUOTE					{ $$ = $2; }
;

%%

void add_device(char* name) {
	devices = (char**)realloc(devices, sizeof(char*) * (count_devices+1));
	devices[count_devices++] = strdup(name);
}

void add_sensor(char* name) {
	sensors = (char**)realloc(sensors, sizeof(char*) * (count_sensors+1));
	sensors[count_sensors++] = strdup(name);
}

bool check_device(char* name) {
	for (int i = 0; i < count_devices; i++) {
		if (strcmp(devices[i], name) == 0)
			return true;
	}

	return false;
}

bool check_sensor(char* name) {
	for (int i = 0; i < count_sensors; i++) {
		if (strcmp(sensors[i], name) == 0)
			return true;
	}

	return false;
}

void yyerror(char *s) {
	fprintf(stderr, "%s, line %d\n", s, yylineno);
	exit(1);
}
