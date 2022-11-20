#include "main.h"
#include "parser.h"


int q_count_devices = -1;
int m_count_devices = 0;
int q_count_events = -1;
int m_count_events = 0;

struct Device *devices = NULL;
struct Event *events = NULL;


void yyerror(char *s) {
	fprintf(stderr, "%s, line %d\n", s, yylineno);
	exit(1);
}


void add_device(char *name) {
	q_count_devices++;

	const struct Device device = {strdup(name), false};
	if (q_count_devices >= m_count_devices) {
		if (m_count_devices == 0)
			m_count_devices++;
		m_count_devices *= 2;
		devices = (struct Device*)realloc(devices, sizeof(struct Device) * m_count_devices);
	}
	devices[q_count_devices] = device;

	write_data_file(device.name, "off");
	logger("INFO", "added a new device", name);
}


int get_index_device(char *name) {
	for (int i = 0; i <= q_count_devices; i++) {
		if (strcmp(devices[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
}


bool check_device(char *name) {
	if (get_index_device(name) != -1)
		return true;
	return false;
}


void turn_on_device(char *name) {	
	const int index = get_index_device(name);
	if (index == -1) 
		yyerror("Failed get index device");

	if (devices[index].job_status)
		printf("[%s] already on\n", name);
	else {
		devices[index].job_status = true;
		printf("[%s] on\n", name);
	}

	logger("INFO", "turn on device", name);
}


void turn_off_device(char *name) {
	const int index = get_index_device(name);
	if (index == -1) 
		yyerror("Failed get index device");

	if (devices[index].job_status) {
		devices[index].job_status = false;
		printf("[%s] off\n", name);
	}
	else
		printf("[%s] already off\n", name);

	logger("INFO", "turn off device", name);
}


void add_event(struct Event *event) {
	q_count_events++;
	printf("[%s] added a new event for %s\n", event->name, event->time);

	if (q_count_events >= m_count_events) {
		if (m_count_events == 0)
			m_count_events++;
		m_count_events *= 2;
		events = (struct Event*)realloc(events, sizeof(struct Event) * m_count_events);
	}
	events[q_count_events] = *event;

	logger("INFO", "added a new event", event->name);
}


void remove_event(int indexToRemove) {
	struct Event *temp = (struct Event*)malloc(sizeof(struct Event) * q_count_events);

	if (indexToRemove != 0)
		memcpy(temp, events, sizeof(struct Event) * indexToRemove);

	if (indexToRemove != q_count_events)
		memcpy(temp+indexToRemove, events+indexToRemove+1, sizeof(struct Event) * (q_count_events - indexToRemove));

	q_count_events--;
	free(events);

	for (int i = 0; i <= q_count_events; i++) {
		events[i] = temp[i];
	}
	free(temp);
}


int get_index_event(char *name) {
	for (int i = 0; i <= q_count_events; i++) {
		if (strcmp(events[i].name, name) == 0)
			return i;
	}
	return -1;
}


void clean_file() {
	FILE *file;
	if (!(file = fopen(G_PATH_FILE_DATA, "w")))
		yyerror("Failed to write data to file");
	fclose(file);
}


void write_data_file(char *name, char *job_status) {
	FILE *file;
	if (!(file = fopen(G_PATH_FILE_DATA, "a")))
		yyerror("Failed to write data to file");

	fprintf(file, "%s %s\n", name, job_status);
	fclose(file);
}


void overwriting_data_file() {
	clean_file();

	for (int i = 0; i <= q_count_devices; i++) {
		if (devices[i].job_status)
			write_data_file(devices[i].name, "on");
		else
			write_data_file(devices[i].name, "off");
	}
}


void logger(char *type, char *action, char *name) {
	FILE *file;

	if (!(file = fopen(G_PATH_FILE_LOG, "a")))
		yyerror("Failed to write data to file");

	const char *time_now = get_time("%d-%m-%Y %H:%M:%S");

	fprintf(file, "[%s][%s] <%s>:%d: %s\n", type, time_now, name, yylineno, action);
	fclose(file);
}


char* get_time(char *format) {
	struct tm *local_time;
	char result[40];
	char *tmp;

	time_t timer = time(NULL);
	local_time = localtime(&timer);

	for (int i = 0; i < 40; i++) result[i] = 0;
	strftime(result, 40, format, local_time);
	tmp = (char*)malloc(sizeof(result));
	strcpy(tmp, result);

	return tmp;
}


void monitoring_events() {
	for (int i = 0; i <= q_count_events; i++) {
		char *time_ = get_time("%H:%M");

		if (strcmp(time_, events[i].time) == 0) {
			if (strcmp(events[i].action, "on") == 0)
				turn_on_device(events[i].name);

			else if (strcmp(events[i].action, "off") == 0)
				turn_off_device(events[i].name);

			const int index = get_index_event(events[i].name);
			if (index == -1) 
				yyerror("Failed get index event");
			remove_event(index);
			overwriting_data_file();
			break;
		}
	}
}


void monitoring() {
	while (true) {
		if (q_count_events != -1)
			monitoring_events();
	}
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
	
	clean_file();
	yyparse();

	//free(devices);

	return 1;
}
