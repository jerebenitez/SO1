#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "baash.h"

int main(int argc, char* argv[]) {
    char *line;
    char **args;
    int status = 1;
    
    // main shell loop
    while (status) {
        printf("%s> ", "test_shell");

        line = get_command();
        args = parse_command(line);

        status = 1;
        free(line);
        free(args);
    }

    // exit from shell
    printf("Bye!");   
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

    if ((tokens = malloc(sizeof(char *) * 128)) == NULL)
        perror("Memory could not be allocated");
        exit(EXIT_FAILURE);

    token = strtok(line, " \t\r\n\a");
    while (token != NULL) {
        tokens[pos++] = token;
        token = strtok(NULL, " \t\r\n\a");
    }

    tokens[pos] = NULL;
    return tokens;
}
