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


struct Device {
	char *name;
	char *actions[3];
	char *states[1];
	bool state;
};

struct Event {
	char *name;
	char *action;
	char *time;
	char *target;
	char *notification;
	bool status_complete;
};

struct Condition {
	char *name;
	//char *time_start;
	//char *time_end;
	struct Event *events;
	int count_events;
};

struct Expression {
	char *name;
	char *time_start;
	char *time_end;
};


void yyerror(char *s);
void processing_actions();

int get_index_device(char *name);
bool check_action_device(char *name, char *action);
bool check_state_device(char *name, char *state);

void turn_on_device(char *name);
void turn_off_device(char *name);
void change_temperature(char *temp);
void boil();
void open_door();
void vacuum();
void sos();
void host_notification(char *notification);

void add_event(struct Event *event);
int get_index_action(char *name);

void clean_file(char *path);
void write_data_file(char *name, char *action, char *time);

char *read_file(char *path);
void configure_devices();
void update_configure();

void logger(char *type, char *action, char *name);
char* get_time(char *format);

void checking_condition(char *name);
void add_condition(struct Expression *expression);
void add_event_condition( struct Event *event);
int get_index_condition(char *name);

void monitoring_events();
void monitoring_condition();
void monitoring();