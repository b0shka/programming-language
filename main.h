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
	char *actions[4];
	char *states[1];
};


struct Device {
	char *name;
	bool job_status;
	bool state;
};

struct Event {
	char *name;
	char *action;
	char *time;
	int target;
};

struct Condition {
	char *name;
	struct Event *event;
};

struct Settings {
	char *name;
	bool state;
};


void yyerror(char *s);
void processing_actions();

int get_index_aviable_device(char *name);
bool check_action_on_aviable(char *name, char *action);
bool check_state_on_aviable(char *name, char *state);

void add_device(char *name);
int get_index_device(char *name);

void turn_on_device(char *name);
void turn_off_device(char *name);
void change_temperature(int temp);
void boil();
void open_door();
void close_door();
void vacuum();

void add_event(struct Event *event);
void remove_event(int indexToRemove);
int get_index_action(char *name);

void clean_file();
void write_data_file(char *name, char *job_status);
void overwriting_data_file();

char *read_configure_file();
void configure_devices();
void add_settings(struct Settings *device);
int get_index_settings_device(char *name);

void logger(char *type, char *action, char *name);
char* get_time(char *format);

bool checking_condition(char *name);
void add_condition(char *name, struct Event *event);

void monitoring_events();
void monitoring_settings();
void monitoring();