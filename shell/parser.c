#include "parser.h"

#include <string.h>
#include <stdlib.h>

extern int last_exit_status;

void debug_print_command(const struct Command *cmd) {
    if (!cmd) {
        printf("Command struct is NULL\n");
        return;
    }

    printf("Command: %s\n", cmd->command ? cmd->command : "NULL");
    
    printf("Arguments (%d):\n", cmd->argc);
    for (int i = 0; i < cmd->argc; i++) {
        printf("  args[%d]: %s\n", i, cmd->args[i] ? cmd->args[i] : "NULL");
    }

    printf("Redirections:\n");
    if (cmd->redirection) {
        printf("  Input file: %s\n", cmd->redirection->input_file ? cmd->redirection->input_file : "NULL");
        printf("  Output file: %s\n", cmd->redirection->output_file ? cmd->redirection->output_file : "NULL");
        printf("  Error file: %s\n", cmd->redirection->error_file ? cmd->redirection->error_file : "NULL");
        printf("  Append output: %d\n", cmd->redirection->append_output);
        printf("  Append error: %d\n", cmd->redirection->append_error);
    } else {
        printf("  No redirections\n");
    }
}


int is_comment(const char *line)
{
    return line[0] == '#';
};

char* expand_variables(const char* token) {
    if (token == NULL) return NULL;
    
    // Handle $? expansion
    if (strcmp(token, "$?") == 0) {
        char* expanded = malloc(16);
        if (expanded) {
            snprintf(expanded, 16, "%d", last_exit_status);
            return expanded;
        }
        return NULL;
    }

    return strdup(token);
}

int is_redirection_char(char c) {
    return c == '<' || c == '>';
}

struct Command *parser(char *line) {
    if(line == NULL || is_comment(line)) {
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
        if (is_redirection_char(token[0]) || (token[0] == '2' && token[1] == '>')) {
            char *filename = token + 1;  
            
            if (token[0] == '2') {
                filename = token + 2;
                if (token[2] == '>') filename++;
                command->redirection->error_file = strdup(filename);
                command->redirection->append_error = (token[2] == '>');
            } 
            else if (token[0] == '>') {
                if (token[1] == '>') {
                    filename++;
                    command->redirection->append_output = 1;
                }
                command->redirection->output_file = strdup(filename);
            }
            else if (token[0] == '<') {
                command->redirection->input_file = strdup(filename);
            }
        } else {
            char *expanded = expand_variables(token);
            if (expanded) {
                command->args[command->argc++] = expanded;
            }
        }
    }

    command->args[command->argc] = NULL;
    return command;
}
