struct Uptime {
  int days;
  int hours;
  int minutes;
  float seconds;
};

void print_usage(void);
void get_file_as_string(char*, char**);

char* get_kernel_version(void);
char* get_seconds_up(void);
char* get_time(void);
char* get_hostname(void);
int get_filesystems(void);
char* get_cpu_model(void);
char* get_cpu_type(void);
float get_load_avg();
char* get_total_mem();
char* get_available_mem();

void get_s_options(void);
void get_interval_duration(int, int);
