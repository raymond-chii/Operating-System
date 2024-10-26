#include "parser.h"

#include <string.h>
#include <stdlib.h>

int is_comment(const char *line)
{
    return line[0] == '#';
};

int is_redirection(const char *token){
    return (strcmp(token, "<") == 0 ||
            strcmp(token, ">") == 0 ||
            strcmp(token, ">>") == 0 ||
            strcmp(token, "2>")  == 0 ||
            strcmp(token, "2>>") == 0
            );
}; 

struct Command *parser(char *line)
{
    if(line == NULL) {
        return NULL;
    }
    if (is_comment(line)) {
        return NULL;
    }
    
    struct Command *command = malloc(sizeof(struct Command));
    if (command == NULL) {
        return NULL;
    }

    command->command = NULL;
    command->args = malloc(64 * sizeof(char*));
    command->argc = 0;
    command->is_builtin = 0;

    command->redirection = malloc(sizeof(struct Redirection));
    if (command->redirection == NULL) {
        free(command->args);
        free(command);
        return NULL;
    }
    command->redirection->input_file = NULL;
    command->redirection->output_file = NULL;
    command->redirection->error_file = NULL;
    command->redirection->append_output = 0;
    command->redirection->append_error = 0;

    char *token = strtok(line, " \t\n");
    if (token == NULL) {
        free(command->redirection);
        free(command->args);
        free(command);
        return NULL;
    }

    command->command = strdup(token);
    command->args[0] = command->command;
    command->argc = 1;


    while ((token = strtok(NULL, " \t\n")) != NULL) {

        if (is_redirection(token)) {
            if (strcmp(token, "<") == 0) {
            command->redirection->input_file = strtok(NULL, " ");
            command->redirection->append_output = 1;
            } else if (strcmp(token, ">") == 0) {
                command->redirection->output_file = strtok(NULL, " ");
                command->redirection->append_output = 0;
            } else if (strcmp(token, "2>") == 0) {
                command->redirection->error_file = strtok(NULL, " ");
                command->redirection->append_error = 1;
            } else if (strcmp(token, ">>") == 0) {
                command->redirection->output_file = strtok(NULL, " ");
                command->redirection->append_output = 1;
            } else if (strcmp(token, "2>>") == 0) {
                command->redirection->error_file = strtok(NULL, " ");
                command->redirection->append_error = 1;
            }
        } else {
            command->args[command->argc] = strdup(token);
            command->argc++;
        }
    }

    command->args[command->argc] = NULL;

    return command; 
};


