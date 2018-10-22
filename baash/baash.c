#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "baash.h"

int main(int argc, char* argv[]) {
    int status = 1, stat_loc;
    pid_t child_pid;
    
    // main shell loop
    while (status) {
        char *line;
        char **args;
        
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("Can't read current directory...");
            return 1;
        }

        printf("%s $ ", cwd);

        line = get_command();
        args = parse_command(line);

        // primitive implementation of exit
        if (args[0] != NULL){
            if (strcmp(args[0], "exit") == 0){
                printf("Bye!\n");
                break;
            } else if (strcmp(args[0], "cd") == 0) {
                chdir(args[1]);
                free(line);
                free(args);
                continue;
            }
        }

        child_pid = fork();
        if (child_pid == 0) {
            invoke(args[0], args);
            // in case execv did return, so as to not have both shells (parent and child) running
            free(line);
            free(args);
            break;
        } else {
            waitpid(child_pid, &stat_loc, WUNTRACED);
        }

        status = 1;
        free(line);
        free(args);
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

char **parse_command(char *line) {
    char **tokens;
    char *token;
    int pos = 0;

    if ((tokens = malloc(sizeof(char *) * BUF_ARGS_SIZE)) == NULL) {
        perror("Memory could not be allocated");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, ARGS_DELIM);
    while (token != NULL) {
        tokens[pos++] = token;
        token = strtok(NULL, ARGS_DELIM);
    }

    tokens[pos] = NULL;
    return tokens;
}

void invoke(char *program, char **args) {
    // if there is an absolute or relative path, the program needn't be looked for
    if (program[0] == '/' || program[0] == '.') {
        if (execv(program, args) < 0)
            printf("baash: %s: no se encontró la orden.\n", program);
    } else {
        // search in $PATH
        char *paths = getenv("PATH");
        char *path;
        char *buf;
        int status = 0;

        path = strtok(paths, ":");
        while (path != NULL) {
            size_t sz;
            sz = snprintf(NULL, 0, "%s/%s", path, program);
            buf = (char *)malloc(sz + 1);
            if (buf == NULL)
                exit(EXIT_FAILURE);
            
            snprintf(buf, sz+1, "%s/%s", path, program);
            if (!(execv(buf, args) < 0)) {
                status = 1;
                break;
            }
            free(buf);
            path = strtok(NULL, ":");
        }

        if(!status)
            if (! (execv(program, args) < 0))
                printf("baash: %s: no se encontró la orden.\n", program);

        free(paths);
        free(path);
        free(buf);
    }
}
