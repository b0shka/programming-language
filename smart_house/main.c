#include "main.h"
#include "parser.h"


int count_devices = 0;
int count_sensors = 0;
int count_variables = 0;
struct Sensor *sensors = NULL;
struct Device *devices = NULL;
struct Variable *variables = NULL;


void add_device(char **name) {
	const struct Device device = {*name, false};
	devices = (struct Device*)realloc(devices, sizeof(struct Device) * (count_devices+1));
	devices[count_devices++] = device;

	write_data_file("DEVICE", name, "off");
	logger("INFO", "add new device", name);
}


void add_sensor(char **name) {
	const struct Sensor sensor = {*name, false};
	sensors = (struct Sensor*)realloc(sensors, sizeof(struct Sensor) * (count_sensors+1));
	sensors[count_sensors++] = sensor;

	write_data_file("SENSOR", name, "off");
	logger("INFO", "add new sensor", name);
}


struct Device* get_device(char **name) {
	int index = get_index_device(name);
	if (index != -1)
		return &devices[index];
}


struct Sensor* get_sensor(char **name) {
	int index = get_index_sensor(name);
	if (index != -1)
		return &sensors[index];
}


int get_index_device(char **name) {
	for (int i = 0; i < count_devices; i++) {
		if (strcmp(devices[i].name, *name) == 0)
			return i;
	}
	return -1;
}


int get_index_sensor(char **name) {
	for (int i = 0; i < count_sensors; i++) {
		if (strcmp(sensors[i].name, *name) == 0)
			return i;
	}
	return -1;
}


bool check_device(char **name) {
	if (get_index_device(name) != -1)
		return true;
	return false;
}


bool check_sensor(char **name) {
	if (get_index_sensor(name) != -1)
		return true;
	return false;
}


void turn_on_device(char **name) {
	const int index = get_index_device(name);

	if (devices[index].job_status)
		printf("[DEVICE %s] already on\n", *name);
	else {
		devices[index].job_status = true;
		printf("[DEVICE %s] on\n", *name);
	}

	logger("INFO", "turn on device", name);
}


void turn_off_device(char **name) {
	const int index = get_index_device(name);

	if (devices[index].job_status) {
		devices[index].job_status = false;
		printf("[DEVICE %s] off\n", *name);
	}
	else
		printf("[DEVICE %s] already off\n", *name);

	logger("INFO", "turn off device", name);
}


void turn_on_sensor(char **name) {
	const int index = get_index_sensor(name);

	if (sensors[index].job_status)
		printf("[SENSOR %s] already on\n", *name);
	else {
		sensors[index].job_status = true;
		printf("[SENSOR %s] on\n", *name);
	}

	logger("INFO", "turn on sensor", name);
}


void turn_off_sensor(char **name) {
	const int index = get_index_sensor(name);

	if (sensors[index].job_status) {
		sensors[index].job_status = false;
		printf("[SENSOR %s] off\n", *name);
	}
	else
		printf("[SENSOR %s] already off\n", *name);

	logger("INFO", "turn off sensor", name);
}


void write_data_file(char *type, char **name, char *job_status) {
	FILE *file;
	if (!(file = fopen(G_PATH_FILE_DATA, "a")))
		yyerror("Failed to write data to file");

	fprintf(file, "%s %s %s\n", type, *name, job_status);
	fclose(file);
}


void overwriting_data_file() {
	FILE *file;
	if (!(file = fopen(G_PATH_FILE_DATA, "w")))
		yyerror("Failed to write data to file");
	fclose(file);

	for (int i = 0; i < count_devices; i++) {
		if (devices[i].job_status)
			write_data_file("DEVICE", &devices[i].name, "on");
		else
			write_data_file("DEVICE", &devices[i].name, "off");
	}

	for (int i = 0; i < count_sensors; i++) {
		if (sensors[i].job_status)
			write_data_file("SENSOR", &sensors[i].name, "on");
		else
			write_data_file("SENSOR", &sensors[i].name, "off");
	}
}


void logger(char *type, char *action, char **name) {
	char *format_time;
	FILE *file;

	if (!(file = fopen(G_PATH_FILE_LOG, "a")))
		yyerror("Failed to write data to file");

	long int time_now = time(NULL);
	format_time = strtok(ctime(&time_now), "\n");

	fprintf(file, "[%s][%s] <%s>:%d: %s\n", type, format_time, *name, yylineno, action);
	fclose(file);
}


void add_var_device(char **name, struct Device *device) {
	const struct Variable var = {*name, device};
	variables = (struct Variable*)realloc(variables, sizeof(struct Variable) * (count_variables+1));
	variables[count_variables++] = var;

	logger("INFO", "add new device variable", &device->name);
}


struct Variable* get_var_by_name(char **name) {
	for (int i = 0; i < count_variables; i++) {
		if (strcmp(variables[i].name, *name) == 0)
			return &variables[i];
	}
}


void yyerror(char *s) {
	fprintf(stderr, "%s, line %d\n", s, yylineno);
	exit(1);
}


int main(int argc, char **argv) {
	if (argc < 2) {
		printf("You didn't specify the file with the code\n");
		exit(1);
	}
	FILE *sourceFile = fopen(argv[1], "r");

	if (!sourceFile) {
		printf("Could not open source file %s\n", argv[1]);
		exit(1);
	}

	yyin = sourceFile;
	yyparse();

	free(devices);
	free(sensors);
	free(variables);

	return 1;
}
