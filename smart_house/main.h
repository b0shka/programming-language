#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "config/config.h"


extern FILE *yyin;
extern int yylineno;
extern int yylex();


struct Device {
	char *name;
	bool job_status;
};

struct Sensor {
	char *name;
	bool job_status;
};

struct Variable {
	char *name;
	struct Device *device;
};


void yyerror(char *s);

void add_device(char **name);
void add_sensor(char **name);
struct Device* get_device(char **name);
struct Sensor* get_sensor(char **name);

int get_index_device(char **name);
int get_index_sensor(char **name);

bool check_device(char **name);
bool check_sensor(char **name);
void turn_on_device(char **name);
void turn_off_device(char **name);
void turn_on_sensor(char **name);
void turn_off_sensor(char **name);

void write_data_file(char *type, char **name, char *job_status);
void overwriting_data_file();

void logger(char *type, char *action, char **name);

void add_var_device(char **name, struct Device *device);
struct Variable* get_var_by_name(char **name);
