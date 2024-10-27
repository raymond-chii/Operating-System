#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

struct Redirection {
    char *input_file;
    char *output_file;
    char *error_file;
    int append_output;
    int append_error;
};

struct Command {
    char *command;
    char **args;
    int argc;
    struct Redirection *redirection;
    int is_builtin;
};

struct Command *parser(char *line);
int is_comment(const char *line);
int is_redirection(const char *token);

void debug_print_command(const struct Command *cmd);

#endif
