%{
#include "main.h"

extern int yylineno;
extern int yylex();

void yyerror (char *s);

int count_devices = 0;
int count_sensors = 0;
char* devices[10];
char* sensors[10];

void add_device(char* name);
void add_sensor(char* name);
void print_devices();
void print_sensors();
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
													if ($3)
														printf("[DEVICE %s] on\n", $2);
													else
														printf("[DEVICE %s] off\n", $2);
												}
|			SENSOR VAL ACTION_JOB				{
													if ($3)
														printf("[SENSOR %s] on\n", $2);
													else
														printf("[SENSOR %s] off\n", $2); 
												}
;

VAL:		'(' QUOTE NAME QUOTE ')'			{ $$ = $3; }
|			QUOTE NAME QUOTE					{ $$ = $2; }
;

%%

void add_device(char* name) {
	devices[count_devices] = name;
	count_devices++;
	print_devices();
}

void add_sensor(char* name) {
	sensors[count_sensors] = name;
	count_sensors++;
	print_sensors();
}

void print_devices() {
	for (int i = 0; i < count_devices; i++) {
		printf("%s ", devices[i]);
	}
	printf("\n");
}

void print_sensors() {
	for (int i = 0; i < count_sensors; i++) {
		printf("%s ", sensors[i]);
	}
	printf("\n");
}

void yyerror(char *s) {
	fprintf(stderr, "%s, line %d\n", s, yylineno);
	exit(1);
}
