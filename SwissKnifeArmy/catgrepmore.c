#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>

#define BUFFER_SIZE 4096
static long total_bytes = 0;
static int total_files = 0;
static int current_file = 0;
static sigjmp_buf env;

static int pipefd[2], pipefd2[2];
static pid_t pid, pid2;

static int pipe1_created = 0;
static int pipe2_created = 0;
static int grep_created = 0;
static int more_created = 0;

void cleanup_processes() {
    int status;
    
    if (pipe1_created) {
        close(pipefd[0]);
        close(pipefd[1]);
        pipe1_created = 0;
    }
    if (pipe2_created) {
        close(pipefd2[0]);
        close(pipefd2[1]);
        pipe2_created = 0;
    }
    if (grep_created) {
        kill(pid, SIGTERM);
        waitpid(pid, &status, 0);
        grep_created = 0;
    }
    if (more_created) {
        kill(pid2, SIGINT);
        waitpid(pid2, &status, 0);
        more_created = 0;
    }
}

void handler(int signal){
    if (signal == SIGUSR1){
        fprintf(stderr, "Status: Processed %d files, %ld bytes\n", 
                total_files, total_bytes);
    } else if (signal == SIGUSR2){

        fprintf(stderr, "Skipping to next file...\n");
        cleanup_processes();
        siglongjmp(env, 2);
    }
}

int establish(int infile_fd, const char *pattern) {
    char buffer[BUFFER_SIZE];
    size_t bytes_read, bytes_written;
    int status;

    // Create first pipe
    if (pipe(pipefd) == -1) {
        perror("pipe1");
        return -1;
    }
    pipe1_created = 1;

    // Create second pipe
    if (pipe(pipefd2) == -1) {
        perror("pipe2");
        cleanup_processes();
        return -1;
    }
    pipe2_created = 1;

    // Fork first child (grep)
    pid = fork();
    if (pid == -1) {
        perror("fork grep");
        cleanup_processes();
        return -1;
    }

    if (pid == 0) {  // grep process
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2 grep stdin");
            _exit(1);
        }
        if (dup2(pipefd2[1], STDOUT_FILENO) == -1) {
            perror("dup2 grep stdout");
            _exit(1);
        }

        close(pipefd[0]);
        close(pipefd[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        execlp("grep", "grep", pattern, (char *)NULL);
        perror("execlp grep");
        _exit(1);
    }
    grep_created = 1;

    // Fork second child (more)
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork more");
        cleanup_processes();
        return -1;
    }

    if (pid2 == 0) {  // more process
        if (dup2(pipefd2[0], STDIN_FILENO) == -1) {
            perror("dup2 pg stdin");
            _exit(1);
        }

        close(pipefd[0]);
        close(pipefd[1]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        execlp("pg", "pg", (char *)NULL);
        perror("execlp pg");
        _exit(1);
    }
    more_created = 1;

    // Parent process
    close(pipefd[0]);   // Close read end of first pipe
    close(pipefd2[0]);  // Close read end of second pipe
    close(pipefd2[1]);  // Close write end of second pipe

    // Read from input file and write to pipe
    while ((bytes_read = read(infile_fd, buffer, BUFFER_SIZE)) > 0) {
        char *write_position = buffer;
        size_t bytes_remaining = bytes_read;
        
        while (bytes_remaining > 0) {
            bytes_written = write(pipefd[1], write_position, bytes_remaining);
            if (bytes_written == -1) {
                if (errno == EINTR)
                    continue;
                if (errno == EPIPE) {
                    cleanup_processes();
                    return 0;
                }
                perror("write");
                cleanup_processes();
                return -1;
            }
            bytes_remaining -= bytes_written;
            write_position += bytes_written;
            total_bytes += bytes_written;
        }
    }

    // Close write end and wait for children
    close(pipefd[1]);
    
    // Wait for both children to finish
    waitpid(pid, &status, 0);
    waitpid(pid2, &status, 0);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s pattern file1 [file2 ...]\n", argv[0]);
        return 1;
    }

    // Set up signal handlers at the start
    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1 || 
        sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    // Ignore SIGPIPE
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) == -1) {
        perror("sigaction SIGPIPE");
        return 1;
    }

    char *pattern = argv[1];
    
    for (int i = 2; i < argc; i++) {
        current_file = i - 1;
        char *infile = argv[i];

        int infile_fd = open(infile, O_RDONLY);
        if (infile_fd == -1) {
            perror(infile);
            continue;
        }

        if (sigsetjmp(env, 1) != 2) {
            if (establish(infile_fd, pattern) == -1) {
                close(infile_fd);
                continue;
            }
        }

        close(infile_fd);
        total_files++;
    }

    return 0;
}
