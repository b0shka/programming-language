#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "config/config.h"


extern FILE *yyin;
extern int yylineno;
extern int yylex();


struct AviableDevice {
	char *name;
	char *actions[3];
	char *states[1];
};


struct Device {
	char *name;
	bool job_status;
};

struct Event {
	char *name;
	char *action;
	char *time;
};


void yyerror(char *s);

int get_index_aviable_device(char *name);
bool check_action_on_aviable(char *name, char *action);
bool check_state_on_aviable(char *name, char *state);

void add_device(char *name);
int get_index_device(char *name);
void turn_on_device(char *name);
void turn_off_device(char *name);

void add_event(struct Event *event);
void remove_event(int indexToRemove);
int get_index_action(char *name);

void clean_file();
void write_data_file(char *name, char *job_status);
void overwriting_data_file();

void logger(char *type, char *action, char *name);
char* get_time(char *format);

void monitoring();
void monitoring_events();