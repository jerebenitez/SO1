#define BUF_ARGS_SIZE 1024
#define ARGS_DELIM " \t\r\n\a"

void main_loop(void);
char *get_command(void);
char **parse_command(char* command);
