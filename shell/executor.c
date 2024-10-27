#include "executor.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>

int execute_command(struct Command *cmd, struct ProcessStats *stats){
    struct timeval start, end;
    struct rusage rusage;
    // pid_t pid;

    gettimeofday(&start, NULL);
    
    stats->pid = fork();
    if (stats->pid == -1) {
        perror("fork");
        return -1;
    }

    if (stats->pid == 0) {  // Child process
        if (cmd->redirection && setup_redirections(cmd->redirection) != 0) {
            _exit(1);
        }

        char *cmd_path = find_command_path(cmd->command);
        if (cmd_path == NULL) {
            fprintf(stderr, "%s: command not found\n", cmd->command);
            _exit(127);
        }

        execv(cmd_path, cmd->args);
        perror(cmd_path);
        free(cmd_path);
        _exit(127);
    }

    // Parent process
    int status;
    if (wait3(&status, 0, &rusage) == -1) {
        perror("wait3");
        return -1;
    }

    gettimeofday(&end, NULL);

    // Calculate times
    stats->real_time = (end.tv_sec - start.tv_sec) + 
                      (end.tv_usec - start.tv_usec) / 1000000.0;
    stats->user_time = rusage.ru_utime.tv_sec + 
                      rusage.ru_utime.tv_usec / 1000000.0;
    stats->sys_time = rusage.ru_stime.tv_sec + 
                      rusage.ru_stime.tv_usec / 1000000.0;

    if (WIFEXITED(status)) {
        stats->exit_status = WEXITSTATUS(status);
        stats->signal_num = 0;
    } else if (WIFSIGNALED(status)) {
        stats->signal_num = WTERMSIG(status);
        stats->exit_status = 128 + stats->signal_num;
    }

    return stats->exit_status;
};

int setup_redirections(struct Redirection *redirection) {

    int input_fd = -1, output_fd = -1, error_fd = -1;
    
    if (redirection->input_file) {
        input_fd = open(redirection->input_file, O_RDONLY);
        if (input_fd == -1) {
            perror(redirection->input_file);
            return 1;
        }
    }

    if (redirection->output_file) {
        int flags = O_WRONLY | O_CREAT;
        flags |= redirection->append_output ? O_APPEND : O_TRUNC;
        
        output_fd = open(redirection->output_file, flags, 0644);
        if (output_fd == -1) {
            if (errno == EACCES) {
                fprintf(stderr, "%s: Permission denied\n", redirection->output_file);
            } else {
                perror(redirection->output_file);
            }
            if (input_fd != -1) close(input_fd);
            return 1;
        }
    }

    if (redirection->error_file) {
        int flags = O_WRONLY | O_CREAT;
        flags |= redirection->append_error ? O_APPEND : O_TRUNC;
        
        error_fd = open(redirection->error_file, flags, 0644);
        if (error_fd == -1) {
            if (errno == EACCES) {
                fprintf(stderr, "%s: Permission denied\n", redirection->error_file);
            } else {
                perror(redirection->error_file);
            }
            if (input_fd != -1) close(input_fd);
            if (output_fd != -1) close(output_fd);
            return 1;
        }
    }

    if (input_fd != -1) {
        if (dup2(input_fd, STDIN_FILENO) == -1) {
            perror("dup2");
            close(input_fd);
            if (output_fd != -1) close(output_fd);
            if (error_fd != -1) close(error_fd);
            return 1;
        }
        close(input_fd);
    }

    if (output_fd != -1) {
        if (dup2(output_fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            close(output_fd);
            if (error_fd != -1) close(error_fd);
            return 1;
        }
        close(output_fd);
    }

    if (error_fd != -1) {
        if (dup2(error_fd, STDERR_FILENO) == -1) {
            perror("dup2");
            close(error_fd);
            return 1;
        }
        close(error_fd);
    }

    return 0;
}

char *find_command_path(const char *command){
    if (strchr(command, '/') != NULL) {
        return strdup(command);
    }

    char *path = getenv("PATH");

    if (path == NULL) {
        return NULL;
    }

    char *path_copy = strdup(path);
    char *dir = strtok(path_copy, ":");
    char *full_path = malloc(PATH_MAX);

    while (dir != NULL) {
        snprintf(full_path, PATH_MAX, "%s/%s", dir, command);
        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return full_path;
        }
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    free(full_path);
    return NULL;

};

void print_process_stats(struct ProcessStats *stats) {
    if (stats->signal_num) {
        fprintf(stderr, "Child process exited with signal %d", stats->signal_num);
        if (stats->signal_num == SIGSEGV)
            fprintf(stderr, " (Segmentation fault)");
        fprintf(stderr, "\n");
    } else {
        if (stats->exit_status != 0)
            fprintf(stderr, "Child process exited with return value %d\n", 
                    stats->exit_status);
        else
            fprintf(stderr, "Child process %d exited normally\n", stats->pid);
    }

    fprintf(stderr, "Real: %.3fs User: %.3fs Sys: %.3fs\n",
            stats->real_time, stats->user_time, stats->sys_time);
}
