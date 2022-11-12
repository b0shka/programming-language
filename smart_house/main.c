#include "main.h"
#include "parser.h"


int count_devices = 0;
int count_sensors = 0;
struct Sensor *sensors = NULL;
struct Device *devices = NULL;


struct Device {
	char *name;
	bool job_status;
};

struct Sensor {
	char *name;
	bool job_status;
};


void add_device(char **name) {
	struct Device device = {*name, false};
	devices = (struct Device*)realloc(devices, sizeof(struct Device) * (count_devices+1));
	devices[count_devices++] = device;
}


void add_sensor(char **name) {
	struct Sensor sensor = {*name, false};
	sensors = (struct Sensor*)realloc(sensors, sizeof(struct Sensor) * (count_sensors+1));
	sensors[count_sensors++] = sensor;
}


bool check_device(char **name) {
	for (int i = 0; i < count_devices; i++) {
		if (strcmp(devices[i].name, *name) == 0)
			return true;
	}
	return false;
}


bool check_sensor(char **name) {
	for (int i = 0; i < count_sensors; i++) {
		if (strcmp(sensors[i].name, *name) == 0)
			return true;
	}
	return false;
}


void turn_on_device(char **name) {
	for (int i = 0; i < count_devices; i++) {
		if (strcmp(devices[i].name, *name) == 0) {
			if (devices[i].job_status)
				printf("[DEVICE %s] already on\n", *name);
			else {
				devices[i].job_status = true;
				printf("[DEVICE %s] on\n", *name);
			}
		}
	}
}


void turn_off_device(char **name) {
	for (int i = 0; i < count_devices; i++) {
		if (strcmp(devices[i].name, *name) == 0) {
			if (devices[i].job_status) {
				devices[i].job_status = false;
				printf("[DEVICE %s] off\n", *name);
			}
			else
				printf("[DEVICE %s] already off\n", *name);
		}
	}
}


void turn_on_sensor(char **name) {
	for (int i = 0; i < count_sensors; i++) {
		if (strcmp(sensors[i].name, *name) == 0) {
			if (sensors[i].job_status)
				printf("[SENSOR %s] already on\n", *name);
			else {
				sensors[i].job_status = true;
				printf("[SENSOR %s] on\n", *name);
			}
		}
	}
}


void turn_off_sensor(char **name) {
	for (int i = 0; i < count_sensors; i++) {
		if (strcmp(sensors[i].name, *name) == 0) {
			if (sensors[i].job_status) {
				sensors[i].job_status = false;
				printf("[SENSOR %s] off\n", *name);
			}
			else
				printf("[SENSOR %s] already off\n", *name);
		}
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
	return yyparse();
}
