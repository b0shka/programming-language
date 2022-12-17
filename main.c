#include "main.h"
#include "parser.h"

#define COUNT_DEVICES 10
#define COUNT_ACTIONS 3
#define COUNT_STATES 1

int q_count_events = -1;
int m_count_events = 0;
int q_count_conditions = -1;
int m_count_conditions = 0;
int q_count_condition_events = -1;
int m_count_condition_events = 0;

struct Event *events = NULL;
struct Event *condition_events = NULL;
struct Condition *conditions = NULL;
struct Device devices[] = {
	"teapot", {"boil"}, {}, false,
	"conditioner", {"on", "off", "target"}, {}, false,
	"doorbell", {}, {"call"}, false,
	"door", {"open"}, {}, false,
	"smoke", {}, {"alarm"}, false,
	"water_leak", {}, {"leak"}, false,
	"speaker", {"sos", "host_notification"}, {}, false,
	"vacuum_cleaner", {"vacuum"}, {}, false,
	"movements", {}, {"movement"}, false,
	"lamp", {"on", "off"}, {}, false
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

	else if (strcmp(event->action, "sos") == 0)
		sos();

	else if (strcmp(event->action, "host_notification") == 0)
		host_notification(event->notification);

	if (event->time != NULL)
		write_data_file(event->name, event->action, event->time);
	else
		write_data_file(event->name, event->action, get_time("%H:%M"));
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


void change_temperature(char *temperature) {
	printf("[conditioner] temperature change by value: %s\n", temperature);
}


void boil() {
	printf("[teapot] boil\n");
}


void open_door() {
	printf("[door] open\n");
}


void vacuum() {
	printf("[vacuum_cleaner] vacuum\n");
}


void sos() {
	printf("[speaker] rescue service called\n");
}


void host_notification(char *notification) {
	printf("[speaker] you have an urgent notification: %s\n", notification);
}


void add_event(struct Event *event) {
	q_count_events++;

	if (q_count_events >= m_count_events) {
		if (m_count_events == 0)
			m_count_events++;
		m_count_events *= 2;
		events = (struct Event*)realloc(events, sizeof(struct Event) * m_count_events);
	}

	events[q_count_events] = *event;
	printf("[%s] added a new event for %s\n", event->name, event->time);
	logger("INFO", "added a new event", event->name);
}


void clean_file(char *path) {
	FILE *file;
	if (!(file = fopen(path, "w")))
		yyerror("Failed to write data to file");
	fclose(file);
}


void write_data_file(char *name, char *action, char *time) {
	FILE *file;
	if (!(file = fopen(G_PATH_FILE_OUTPUT, "a")))
		yyerror("Failed to write data to file");

	fprintf(file, "[%s] %s %s\n", time, name, action);
	fclose(file);
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
	char *data = read_file(G_PATH_FILE_CONFIGURE);
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


void update_configure() {
	clean_file(G_PATH_FILE_CONFIGURE);

	FILE *file;
	if (!(file = fopen(G_PATH_FILE_CONFIGURE, "a")))
		yyerror("Failed to write data to file");

	char *status;

	for (int i = 0; i < COUNT_DEVICES; i++) {
		if (devices[i].states[0]) {
			if (devices[i].state)
				status = "true";
			else
				status = "false";
			fprintf(file, "%s %s\n", devices[i].name, status);
		}
	}

	fclose(file);
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


void add_condition(struct Arguments *arguments) {
	q_count_conditions++;

	struct Condition *condition = (struct Condition*)malloc(sizeof(struct Condition));
	condition->name = arguments->name;
	condition->time_start = arguments->time_start;
	condition->time_end = arguments->time_end;
	condition->events = condition_events;
	condition->count_events = q_count_condition_events+1;

	if (q_count_conditions >= m_count_conditions) {
		if (m_count_conditions == 0)
			m_count_conditions++;
		m_count_conditions *= 2;
		conditions = (struct Condition*)realloc(conditions, sizeof(struct Condition) * m_count_conditions);
	}

	conditions[q_count_conditions] = *condition;
	printf("[%s] added a new condition\n", arguments->name);
	logger("INFO", "added a new condition", arguments->name);

	condition_events = NULL;
	q_count_condition_events = -1;
	m_count_condition_events = 0;
}


void add_event_condition(struct Event *event) {
	q_count_condition_events++;

	if (q_count_condition_events >= m_count_condition_events) {
		if (m_count_condition_events == 0)
			m_count_condition_events++;
		m_count_condition_events *= 2;
		condition_events = (struct Event*)realloc(condition_events, sizeof(struct Event) * m_count_condition_events);
	}

	condition_events[q_count_condition_events] = *event;
	logger("INFO", "added a new event to condition", event->name);
}


bool time_comparison(char *time_now, char *time_start, char *time_end) {
	bool result = false;
	int hours_time_now = get_hours_from_time(time_now);
	int minutes_time_now = get_minutes_from_time(time_now);
	int hours_time_start, hours_time_end, minutes_time_start, minutes_time_end;

	if (time_start != NULL && time_end != NULL) {
		hours_time_start = get_hours_from_time(time_start);
		hours_time_end = get_hours_from_time(time_end);

		if (hours_time_start > hours_time_end) {
			if (hours_time_now > hours_time_start || hours_time_now < hours_time_end)
				result = true;

			else if (hours_time_now == hours_time_start) {
				minutes_time_start = get_minutes_from_time(time_start);
				if (minutes_time_now > minutes_time_start)
					result = true;
			}

			else if (hours_time_now == hours_time_end) {
				minutes_time_end = get_minutes_from_time(time_end);
				if (minutes_time_now < minutes_time_end)
					result = true;
			}
		}

		else if (hours_time_start < hours_time_end) {
			if (hours_time_now > hours_time_start && hours_time_now < hours_time_end)
				result = true;

			else if (hours_time_now == hours_time_start) {
				minutes_time_start = get_minutes_from_time(time_start);
				if (minutes_time_now > minutes_time_start)
					result = true;
			}

			else if (hours_time_now == hours_time_end) {
				minutes_time_end = get_minutes_from_time(time_end);
				if (minutes_time_now < minutes_time_end)
					result = true;
			}
		}

		else {
			if (hours_time_now == hours_time_end) {
				minutes_time_start = get_minutes_from_time(time_start);
				minutes_time_end = get_minutes_from_time(time_end);

				if (minutes_time_now > minutes_time_start && minutes_time_now < minutes_time_end)
					result = true;
			}
		}
	}

	else if (time_start != NULL) {
		hours_time_start = get_hours_from_time(time_start);

		if (hours_time_now > hours_time_start)
			result = true;
		else if (hours_time_now == hours_time_start) {
			minutes_time_start = get_minutes_from_time(time_start);

			if (minutes_time_now >= minutes_time_start)
				result = true;
		}
	}

	else if (time_end != NULL) {
		hours_time_end = get_hours_from_time(time_end);

		if (hours_time_now < hours_time_end)
			result = true;
		else if (hours_time_now == hours_time_end) {
			minutes_time_end = get_minutes_from_time(time_end);

			if (minutes_time_now <= minutes_time_end)
				result = true;
		}
	}

	else
		result = true;
	
	return result;
}


int get_hours_from_time(char *origin_time) {
	char result[2];

	for (int i = 0; i < 3; i++) {
		if (origin_time[i] != ':')
			result[i] = origin_time[i];
		else {
			if (i == 1) {
				result[1] = result[0];
				result[0] = '0';
			}
			break;
		}
	}
	return atoi(result);
}


int get_minutes_from_time(char *origin_time) {
	char result[2];
	bool mode = false;

	for (int i = 0; i < 5; i++) {
		if (mode) {
			if (origin_time[2] == ':')
				result[i-3] = origin_time[i];
			else
				result[i-2] = origin_time[i];
		}
		if (origin_time[i] == ':')
			mode = true;
	}
	return atoi(result);
}


void monitoring_events() {
	//char *time_ = get_time("%H:%M");
	char *time_ = read_file(G_PATH_FILE_TIME);

	if (time_[4] == '\n')
		time_[4] = '\0';
	else
		time_[5] = '\0';

	for (int i = 0; i <= q_count_events; i++) {
		if (strcmp(time_, events[i].time) == 0 && !events[i].status_complete) {
			processing_actions(&events[i]);
			events[i].status_complete = true;
		}
		else if (strcmp(time_, events[i].time) != 0 && events[i].status_complete)
			events[i].status_complete = false;
	}
}


void monitoring_condition() {
	int index;
	bool time_compare = true;
	char *time_ = read_file(G_PATH_FILE_TIME);

	for (int i = 0; i <= q_count_conditions; i++) {
		index = get_index_device(conditions[i].name);
		if (index == -1)
			yyerror("Failed get index device");

		time_compare = time_comparison(time_, conditions[i].time_start, conditions[i].time_end);

		if (devices[index].state && time_compare) {
			for (int j = 0; j < conditions[i].count_events; j++)
				processing_actions(&conditions[i].events[j]);

			devices[index].state = false;
			update_configure();
		}
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
	clean_file(G_PATH_FILE_OUTPUT);
	yyparse();

	free(events);
	free(conditions);
	free(condition_events);

	return 1;
}
