#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "baash.h"

int main(int argc, char* argv[]) {
    char *line;
    char **args;
    int status = 1, stat_loc;
    pid_t child_pid;
    
    // main shell loop
    while (status) {
        printf("%s> ", "test_shell");

        line = get_command();
        args = parse_command(line);

        // primitive implementation of exit
        if (!strcmp(args[0], "exit"))
            break;

        child_pid = fork();
        if (child_pid == 0) {
            invoke(args[0], args);
        } else {
            waitpid(child_pid, &stat_loc, WUNTRACED);
        }

        status = 1;
        free(line);
        free(args);
    }

    // exit from shell
    printf("Bye!\n");   
    return EXIT_SUCCESS;
}

char *get_command() {
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, stdin);

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

void invoke(char *program, char **argv) {
    if (program[0] == '\\') {
        // the path is absolute
    } else if (program[0] == '.') {
        // the path is relative
    } else {
        // search in $PATH
    }
}
