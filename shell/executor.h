#ifndef EXECUTER_H
#define EXECUTER_H

#include "parser.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>

struct ProcessStats {
    double real_time;
    double user_time;
    double sys_time;
    int exit_status;
    int signal_num;
    pid_t pid;
};


int execute_command(struct Command *cmd, struct ProcessStats *stats);
int setup_redirections(struct Redirection *redirection);
char *find_command_path(const char *command);
void print_process_stats(struct ProcessStats *stats);

#endif
