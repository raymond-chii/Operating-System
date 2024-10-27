#include "parser.h"
#include "executor.h"
#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int last_exit_status = 0;

void process_line(char *line) {
    struct Command *cmd;
    struct ProcessStats stats;
    int result;

    if (line == NULL || line[0] == '\n' || line[0] == '\0') {
        return;
    }
    line[strcspn(line, "\n")] = 0;
    cmd = parser(line);
    if (cmd == NULL) {
        return;
    }

    // debug_print_command(cmd);

    if (is_builtin(cmd->command)) {
        last_exit_status = execute_builtin(cmd);
    } else {
        result = execute_command(cmd, &stats);
        print_process_stats(&stats);
        last_exit_status = result;
    }

    
    if (cmd->args) {
        for (int i = 0; i < cmd->argc; i++) {
            free(cmd->args[i]);
        }
        free(cmd->args);
    }
    if (cmd->redirection) {
        free(cmd->redirection->input_file);
        free(cmd->redirection->output_file);
        free(cmd->redirection->error_file);
        free(cmd->redirection);
    }
    free(cmd);
}

int main(int argc, char *argv[]) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    FILE *input = stdin;

    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror(argv[1]);
            exit(1);
        }
    }

    while ((nread = getline(&line, &len, input)) != -1) {
        process_line(line);
    }

    // Handle EOF
    if (input != stdin) {
        fclose(input);
    }
    free(line);

    fprintf(stderr, "end of file read, exiting shell with exit code %d\n", last_exit_status);
    exit(last_exit_status);
}
