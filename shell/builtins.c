#include "builtins.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

extern int last_exit_status;


int is_builtin(const char *command){
    return (
        strcmp(command, "cd") == 0 ||
        strcmp(command, "pwd") == 0 ||
        strcmp(command, "exit") == 0
    );
};

int builtin_cd(struct Command *cmd){

    if (cmd->args[1] == NULL) {
        if (chdir(getenv("HOME")) != 0) {
            perror(getenv("HOME"));
            return 1;
        }
        return 0;
    }

    if (chdir(cmd->args[1]) != 0) {
        printf("cd: %s: No such file or directory\n", cmd->args[1]);
        return 1;
    }
    return 0;
};

int builtin_pwd(struct Command *cmd){

    char cwd[1024];

    if (cmd->argc > 1) {
        fprintf(stderr, "pwd: too many arguments\n");
        return 1;
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return 1;
    }

    printf("%s\n", cwd);
    return 0;
};

int builtin_exit(struct Command *cmd) {
    int status;
    
    if (cmd->argc > 2) {
        fprintf(stderr, "exit: too many arguments\n");
        return 1;
    }

    if (cmd->argc == 1) {
        exit(last_exit_status);
    }

    char *endptr;
    status = strtol(cmd->args[1], &endptr, 10);

    if (*endptr != '\0') {
        fprintf(stderr, "exit: %s: numeric argument required\n", cmd->args[1]);
        exit(255);
    }

    exit(status & 0xFF);
}

int execute_builtin(struct Command *cmd) {
    if (!is_builtin(cmd->command)) {
        return -1;
    }
    if (strcmp(cmd->command, "cd") == 0) {
        return builtin_cd(cmd);
    } else if (strcmp(cmd->command, "pwd") == 0) {
        return builtin_pwd(cmd);
    } else if (strcmp(cmd->command, "exit") == 0) {
        return builtin_exit(cmd);
    }

    return -1;
};
