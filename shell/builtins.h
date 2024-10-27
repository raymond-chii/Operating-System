#ifndef BUILTINS_H
#define BUILTINS_H

#include "parser.h"

int execute_builtin(struct Command *cmd);
int is_builtin(const char *command);

int builtin_cd(struct Command *cmd);
int builtin_pwd(struct Command *cmd);
int builtin_exit(struct Command *cmd);


#endif
