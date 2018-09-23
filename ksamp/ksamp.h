struct Uptime {
  int days;
  int hours;
  int minutes;
  float seconds;
};

char* get_kernel_version(void);
char* get_seconds_up(void);
char* get_time(void);
char* get_hostname(void);
int get_filesystems(void);
char* get_cpu_model(void);
char* get_cpu_type(void);
void get_s_options(void);
