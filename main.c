#include "main.h"
#include "parser.h"


#define COUNT_DEVICES 8
#define COUNT_ACTIONS 4
#define COUNT_STATES 1

int q_count_devices = -1;
int m_count_devices = 0;
int q_count_events = -1;
int m_count_events = 0;
int q_count_settings = -1;
int m_count_settings = 0;
int q_count_conditions = -1;
int m_count_conditions = 0;

struct Device *devices = NULL;
struct Event *events = NULL;
struct Settings *settings_devices = NULL;
struct Condition *conditions = NULL;
struct AviableDevice aviable_devices[] = {
	"teapot", {"boil"}, {},
	"conditioner", {"on", "off", "target"}, {},
	"doorbell", {"on", "off"}, {"call"},
	"door", {"on", "off", "open", "close"}, {},
	"temperature", {"on", "off"}, {"get_value"},
	"smoke", {"on", "off"}, {"alarm"},
	"speaker", {"on", "off", "sos"}, {},
	"vacuum_cleaner", {"vacuum"}, {},
};


void yyerror(char *s) {
	fprintf(stderr, "%s, line %d\n", s, yylineno);
	exit(1);
}


void processing_actions(struct Event *event) {
	if (check_action_on_aviable(event->name, event->action)) {
		if (event->time != NULL)
			add_event(event);
		else {
			if (strcmp(event->action, "on") == 0)
				turn_on_device(event->name);

			else if (strcmp(event->action, "off") == 0)
				turn_off_device(event->name);

			else if (strcmp(event->action, "boil") == 0)
				boil();

			else if (strcmp(event->action, "open") == 0)
				open_door();

			else if (strcmp(event->action, "close") == 0)
				close_door();

			else if (strcmp(event->action, "vacuum") == 0)
				vacuum();

			else if (strcmp(event->action, "target") == 0)
				change_temperature(event->target);

			overwriting_data_file();
		}
	}
	else
		yyerror("This action is not supported");
}


int get_index_aviable_device(char *name) {
	for (int i = 0; i < COUNT_DEVICES; i++) {
		if (strcmp(aviable_devices[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
}


bool check_action_on_aviable(char *name, char *action) {
	int index = get_index_aviable_device(name);

	for (int i = 0; i < COUNT_ACTIONS; i++) {
		if (aviable_devices[index].actions[i] && strcmp(aviable_devices[index].actions[i], action) == 0)
			return true;
	}
	return false;
}


bool check_state_on_aviable(char *name, char *state) {
	int index = get_index_aviable_device(name);

	for (int i = 0; i < COUNT_STATES; i++) {
		if (aviable_devices[index].states[i] && strcmp(aviable_devices[index].states[i], state) == 0)
			return true;
	}
	return false;
}


void add_device(char *name) {
	if (get_index_aviable_device(name) != -1) {
		q_count_devices++;

		struct Device device = {strdup(name), false};
		if (q_count_devices >= m_count_devices) {
			if (m_count_devices == 0)
				m_count_devices++;
			m_count_devices *= 2;
			devices = (struct Device*)realloc(devices, sizeof(struct Device) * m_count_devices);
		}

		int index = get_index_settings_device(name);
		if (index != -1)
			device.state = settings_devices[index].state;

		devices[q_count_devices] = device;

		write_data_file(device.name, "off");
		logger("INFO", "added a new device", name);
	}
	else
		yyerror("Such a device is not supported");
}


int get_index_device(char *name) {
	for (int i = 0; i <= q_count_devices; i++) {
		if (strcmp(devices[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
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


void change_temperature(int temperature) {
	printf("[conditioner] temperature change by value: %d\n", temperature);
}


void boil() {
	printf("[teapot] start boil\n");
}


void open_door() {
	printf("[door] open\n");
}


void close_door() {
	printf("[door] close\n");
}


void vacuum() {
	printf("[vacuum_cleaner] vacuum\n");
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
	if (!(file = fopen(G_PATH_FILE_OUTPUT, "w")))
		yyerror("Failed to write data to file");
	fclose(file);
}


void write_data_file(char *name, char *job_status) {
	FILE *file;
	if (!(file = fopen(G_PATH_FILE_OUTPUT, "a")))
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


char *read_configure_file() {
	char symbol;
	char *data = NULL;
	int q_count_chars = 0;
	int m_count_chars = 0;

	FILE *file;
	if (!(file = fopen(G_PATH_FILE_INPUT, "r")))
		printf("Не удалось открыть файл\n");

	do {
		if (q_count_chars >= m_count_chars) {
			if (m_count_chars == 0)
				m_count_chars++;
			m_count_chars *= 2;
			data = (char*)realloc(data, sizeof(char) * m_count_chars);
		}
		symbol = fgetc(file);
		data[q_count_chars] = symbol;
		q_count_chars++;
	} while (symbol != EOF); 

	fclose(file);
	data[q_count_chars - 1] = '\0';
	return data;
}


void configure_devices() {
	char *data = read_configure_file();
	char *data_device = NULL;
	char *data_detail_device = NULL;
	char *end_device, *name;
	int index, mode_detail = 0;
	bool state;

	for (
		data_device = strtok_r(data, "\n", &end_device); 
		data_device != NULL; 
		data_device = strtok_r(NULL, "\n", &end_device)
	) {
		char *end_string;
		char *p = strchr(data_device, '\n');
		if (p != NULL) {
			memmove(data_device, p + 1, strlen(p));
		}

		for (
			data_detail_device = strtok_r(data_device, " ", &end_string);
			data_detail_device != NULL; 
			data_detail_device = strtok_r(NULL, " ", &end_string)
		) {
			if (mode_detail == 0) {
				name = data_detail_device;
				mode_detail = 1;
			}
			else if (mode_detail == 1) {
				if (get_index_aviable_device(name) != -1) {
					if (strcmp(data_detail_device, "true") == 0)
						state = true;
					else
						state = false;
					struct Settings device = {strdup(name), state};
					add_settings(&device);
				}
				mode_detail = 0;
			}
		}
	}
}


void add_settings(struct Settings *device) {
	q_count_settings++;

	if (q_count_settings >= m_count_settings) {
		if (m_count_settings == 0)
			m_count_settings++;
		m_count_settings *= 2;
		settings_devices = (struct Settings*)realloc(settings_devices, sizeof(struct Settings) * m_count_settings);
	}
	settings_devices[q_count_settings] = *device;

	logger("INFO", "added a new settings", device->name);
}


int get_index_settings_device(char *name) {
	for (int i = 0; i <= q_count_settings; i++) {
		if (strcmp(settings_devices[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
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


bool checking_condition(char *name) {
	int index = get_index_device(name);
	if (index != -1) {
		if (devices[index].state)
			return true;
		return false;
	}
	else
		yyerror("Failed get index device");
}


void add_condition(char *name, struct Event *event) {
	q_count_conditions++;
	struct Condition condition = {strdup(name), event};

	if (q_count_conditions >= m_count_conditions) {
		if (m_count_conditions == 0)
			m_count_conditions++;
		m_count_conditions *= 2;
		conditions = (struct Condition*)realloc(conditions, sizeof(struct Condition) * m_count_conditions);
	}
	conditions[q_count_conditions] = condition;

	logger("INFO", "added a new condition", name);
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


void monitoring_settings() {

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

	configure_devices();
	clean_file();
	yyparse();

	free(devices);
	return 1;
}
