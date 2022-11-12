#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

extern FILE *yyin;
extern int yylineno;
extern int yylex();

void yyerror(char *s);

void add_device(char **name);
void add_sensor(char **name);
void get_device(char **name);
void get_sensor(char **name);

int get_index_device(char **name);
int get_index_sensor(char **name);

bool check_device(char **name);
bool check_sensor(char **name);
void turn_on_device(char **name);
void turn_off_device(char **name);
void turn_on_sensor(char **name);
void turn_off_sensor(char **name);

void write_data_file(char *type, char *name, char *job_status);
