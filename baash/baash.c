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

void invoke(char *program, char **args) {
    // if there is an absolute or relative path, the program needn't be looked for
    if (program[0] == '/' || program[0] == '.') {
        execv(program, args);
    } else {
        // search in $PATH
        char *paths = getenv("PATH");
        char *path;
        char *buf;
        size_t sz;

        path = strtok(paths, ":");
        while (path != NULL) {
            //printf("%s\n", path);
            sz = snprintf(NULL, 0, "%s/%s", path, program);
            buf = (char *)malloc(sz + 1);
            if (buf == NULL)
                exit(EXIT_FAILURE);
            snprintf(buf, sz+1, "%s/%s", path, program);
            execv(buf, args);
            free(buf);
            path = strtok(NULL, ":");
        }

        free(paths);
        free(path);
        free(buf);
    }
}
