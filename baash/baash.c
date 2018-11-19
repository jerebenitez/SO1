#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

// Constants
#define BUF_ARGS_SIZE 1024
#define ARGS_DELIM " \t\r\n\a"

// Nodo for commands linked list
typedef struct command {
    char *command;
    char **argv;
    int argc;
    int is_concurrent;
    int is_piped;
    char *input;
    char *output;

    struct command *next;
} command_node;

// Prototypes
void main_loop(void);
char *get_command(void);
void parse_command(char*, command_node*);
void invoke(char*, char**);

char is_delim(char*);
command_node *new_node();

int main(int argc, char* argv[]) {
    int status = 1, stat_loc;
    pid_t child_pid;
    
    // main shell loop
    while (status) {
        char *line;
        command_node *start = new_node();
        
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("Can't read current directory...");
            return 1;
        }
        
        char *user = getenv("USER");
        char host[BUF_ARGS_SIZE];
        gethostname(host, BUF_ARGS_SIZE);
        printf("%s@%s  %s$ ", user, host, cwd);

        line = get_command();
        parse_command(line, start);

        while (start != NULL) {
            child_pid = fork();

            if (child_pid < 0) {
                perror("Error in fork.\n");
                exit(EXIT_FAILURE);
            }
            if (child_pid == 0) {
                invoke(start->command, start->argv);
                // in case execv did return, so as to not have both shells (parent and child) running
                free(line);
                break;
            } else {
                if (!start->is_concurrent)
                    waitpid(child_pid, &stat_loc, WUNTRACED);
            }

            start = start->next;
        }


        status = 1;
        free(line);
    }

    // exit from shell
    return EXIT_SUCCESS;
}

char *get_command() {
    size_t len = 32;
    char *line = (char *)malloc(len * sizeof(char));
    if (getline(&line, &len, stdin) == -1) {
        perror("Could not read line");
        exit(EXIT_FAILURE);
    }
    
    return line;
}

void parse_command(char *line,  command_node *head) {
    command_node *node = head; 
    char *token;
    int pos = 0;

    token = strtok(line, ARGS_DELIM);
    int delim_required = 0;

    while (token != NULL) {
        char delim = is_delim(token);
        
        if (delim_required && delim < 0) {
            perror("Syntax error.\n");
            exit(EXIT_FAILURE);
        }
        
        if (pos == 0 && delim < 0) {
            node->command = token;
        } else if (pos == 0 && delim >= 0 && delim != ';') {
            perror("Syntax error.\n");
            exit(EXIT_FAILURE);
        }

        if (delim < 0) {
            // get size of token
            size_t sz_token = snprintf(NULL, 0, "%s", token);
            // alloc memory to store it
            node->argv[pos] = (char *)malloc((sz_token + 1) * sizeof(char));
            // assign it
            sprintf(node->argv[pos], token);
            pos++;
            // increment number of arguments
            node->argc++;
        } else {
            switch (delim) {
                case '|':
                    node->is_piped = 1;
                    delim_required = 0;
                    node->next = new_node();
                    node = node->next;
                    pos = 0;
                    break;
                case '&':
                    node->is_concurrent = 1;
                    delim_required = 0;
                    node->next = new_node();
                    node = node->next;
                    pos = 0;
                    break;
                case '>':
                    node->output = strtok(NULL, ARGS_DELIM);
                    if (node->output == NULL)
                        exit(EXIT_FAILURE);
                    delim_required  = 1;
                    break;
                case '<':
                    node->input = strtok(NULL, ARGS_DELIM);
                    if (node->input == NULL)
                        exit(EXIT_FAILURE);
                    delim_required = 1;
                    break;
                default:
                    pos = 0;
                    delim_required = 0;
                    node->next = new_node();
                    node = node->next;
                    break;
            } 
        }

        token = strtok(NULL, ARGS_DELIM);
    }
}

char is_delim(char *string) {
    char *SPECIAL_CHARS[] = {"|", ">", "<", "&", ";"};
    int SPECIAL_CHARS_LEN = sizeof(SPECIAL_CHARS)/sizeof(SPECIAL_CHARS[0]);
    
    for (int i = 0; i < SPECIAL_CHARS_LEN; i++) {
        if (strcmp(string, SPECIAL_CHARS[i]) == 0)
            return SPECIAL_CHARS[i][0];
    }

    return -1;
}

command_node *new_node() {
    command_node *node = malloc(sizeof(command_node));
    if (node == NULL)
        exit(EXIT_FAILURE);  

    node->command = NULL;
    // argv can have a maximum of 100 elements
    node->argv = (char **)malloc(100*sizeof(char *));
    node->argc = 0;
    node->is_concurrent = 0;
    node->is_piped = 0;
    node->input = NULL;
    node->output = NULL;
    node->next=NULL;

    return node;
}

void invoke(char *program, char **args) {
    
    // check if program is an internal command
    if (strcmp(program, "exit") == 0){
        printf("Bye!\n");
        kill(getppid(), 9);
        exit(EXIT_SUCCESS);
    } else if (strcmp(program, "cd") == 0) {
        chdir(args[1]);
        return;
    }
    
    // if there is an absolute or relative path, the program needn't be looked for
    if (program[0] == '/' || program[0] == '.') {
        if (execv(program, args) < 0)
            printf("baash: %s: no se encontró la orden.\n", program);
    } else {
        // search in $PATH
        char *paths = getenv("PATH");
        char *path;
        int status = 1;

        path = strtok(paths, ":");
        
        /*
         * Agrego program al final de cada entrada de $PATH y llamo a execv con eso para ver si
         * lo encuentra y lo puede ejecutar. Si no, va a retornar un -1 que va a cambiar status
         */
        while (path != NULL) {
            size_t sz = snprintf(NULL, 0, "%s/%s", path, program);
            char *buf = (char *)malloc(sz + 1);
            if (buf == NULL)
                exit(EXIT_FAILURE);
            
            snprintf(buf, sz+1, "%s/%s", path, program);
            
            // Si execv retorna, es porque no pudo ejecutar el programa
            if ((execv(buf, args) < 0))
                status = 0;
            
            path = strtok(NULL, ":");
            free(buf);
        }

        /*
         * Si al recorrer $PATH no se ejecutó el programa, intento ejecutarlo directamente por si
         * la direcció es relativa pero tiene el ./ implícito.
         * Ej: $HOME/SO1 $ ksamp/ksamp (equivalente a ./ksamp/ksamp)
         */
        if(!status)
            if ((execv(program, args) < 0))
                printf("baash: %s: no se encontró la orden.\n", program);

        free(paths);
        free(path);
    }
}
