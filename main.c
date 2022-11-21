#include "main.h"
#include "parser.h"


#define COUNT_DEVICES 8
#define COUNT_ACTIONS 3
#define COUNT_STATES 1

int q_count_events = -1;
int m_count_events = 0;
int q_count_conditions = -1;
int m_count_conditions = 0;

struct Event *events = NULL;
struct Condition *conditions = NULL;
struct Device devices[] = {
	"teapot", {"boil"}, {}, false,
	"conditioner", {"on", "off", "target"}, {}, false,
	"doorbell", {}, {"call"}, false,
	"door", {"open"}, {}, false,
	"temperature", {}, {"get_value"}, false,
	"smoke", {}, {"alarm"}, false,
	"speaker", {"sos"}, {}, false,
	"vacuum_cleaner", {"vacuum"}, {}, false,
};


void yyerror(char *s) {
	fprintf(stderr, "%s, line %d\n", s, yylineno);
	exit(1);
}


void processing_actions(struct Event *event) {
	if (strcmp(event->action, "on") == 0)
		turn_on_device(event->name);

	else if (strcmp(event->action, "off") == 0)
		turn_off_device(event->name);

	else if (strcmp(event->action, "boil") == 0)
		boil();

	else if (strcmp(event->action, "open") == 0)
		open_door();

	else if (strcmp(event->action, "vacuum") == 0)
		vacuum();

	else if (strcmp(event->action, "target") == 0)
		change_temperature(event->target);

	//overwriting_data_file();
}


int get_index_device(char *name) {
	for (int i = 0; i < COUNT_DEVICES; i++) {
		if (strcmp(devices[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
}


bool check_action_device(char *name, char *action) {
	int index = get_index_device(name);

	for (int i = 0; i < COUNT_ACTIONS; i++) {
		if (devices[index].actions[i] && strcmp(devices[index].actions[i], action) == 0)
			return true;
	}
	return false;
}


bool check_state_device(char *name, char *state) {
	int index = get_index_device(name);

	for (int i = 0; i < COUNT_STATES; i++) {
		if (devices[index].states[i] && strcmp(devices[index].states[i], state) == 0)
			return true;
	}
	return false;
}


void turn_on_device(char *name) {	
	const int index = get_index_device(name);
	if (index == -1) 
		yyerror("Failed get index device");

	printf("[%s] on\n", name);
	logger("INFO", "turn on device", name);
}


void turn_off_device(char *name) {
	const int index = get_index_device(name);
	if (index == -1) 
		yyerror("Failed get index device");

	printf("[%s] off\n", name);
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

	for (int i = 0; i <= COUNT_DEVICES; i++) {
		//if (devices[i].job_status)
		//	write_data_file(devices[i].name, "on");
		//else
		//	write_data_file(devices[i].name, "off");
	}
}


char* read_file(char *path) {
	char symbol;
	char *data = NULL;
	int q_count_chars = 0;
	int m_count_chars = 0;

	FILE *file;
	if (!(file = fopen(path, "r")))
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
	char *data = read_file(G_PATH_FILE_INPUT);
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
				if (get_index_device(name) != -1) {
					if (strcmp(data_detail_device, "true") == 0)
						state = true;
					else
						state = false;

					int index = get_index_device(strdup(name));
					if (index != -1)
						devices[index].state = state;
				}
				mode_detail = 0;
			}
		}
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
	//char *time_ = get_time("%H:%M");
	char *time_ = read_file("time.txt");
	time_[5] = '\0';

	for (int i = 0; i <= q_count_events; i++) {
		if (strcmp(time_, events[i].time) == 0)
			processing_actions(&events[i]);
	}
}


void monitoring_condition() {
	int index;

	for (int i = 0; i <= q_count_conditions; i++) {
		index = get_index_device(conditions[i].name);
		if (index == -1)
			yyerror("Failed get index device");

		if (devices[index].state)
			processing_actions(conditions[i].event);
	}
}


void monitoring() {
	while (true) {
		monitoring_events();
		configure_devices();
		monitoring_condition();
		sleep(3);
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

	free(events);
	free(conditions);

	return 1;
}
