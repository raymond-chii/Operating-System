#include "parser.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

void print_command(struct Command *cmd, const char *test_name) {
    printf("\n=== Testing: %s ===\n", test_name);
    if (cmd == NULL) {
        printf("Command is NULL\n");
        return;
    }

    printf("Command: %s\n", cmd->command);
    printf("Arguments (%d):\n", cmd->argc);
    for (int i = 0; i < cmd->argc; i++) {
        printf("  args[%d]: %s\n", i, cmd->args[i]);
    }

    printf("Redirections:\n");
    if (cmd->redirection) {
        if (cmd->redirection->input_file)
            printf("  Input: %s\n", cmd->redirection->input_file);
        if (cmd->redirection->output_file)
            printf("  Output: %s (append: %d)\n", 
                   cmd->redirection->output_file, 
                   cmd->redirection->append_output);
        if (cmd->redirection->error_file)
            printf("  Error: %s (append: %d)\n", 
                   cmd->redirection->error_file, 
                   cmd->redirection->append_error);
    }
}

void free_command(struct Command *cmd) {
    if (cmd == NULL) return;
    
    if (cmd->command) {
        free(cmd->command);
    }
    
    if (cmd->args) {
        for (int i = 1; i < cmd->argc; i++) {
            if (cmd->args[i]) {
                free(cmd->args[i]);
            }
        }
        free(cmd->args);  // Free the array itself
    }
    
    if (cmd->redirection) {
        if (cmd->redirection->input_file) {
            free(cmd->redirection->input_file);
        }
        if (cmd->redirection->output_file) {
            free(cmd->redirection->output_file);
        }
        if (cmd->redirection->error_file) {
            free(cmd->redirection->error_file);
        }
        free(cmd->redirection);
    }
    
    free(cmd);
}

void verify_command(struct Command *cmd, const char *expected_command, 
                   int expected_argc, char **expected_args,
                   const char *test_name) {
    printf("\nVerifying %s...\n", test_name);
    assert(cmd != NULL);
    assert(strcmp(cmd->command, expected_command) == 0);
    assert(cmd->argc == expected_argc);
    
    for (int i = 0; i < expected_argc; i++) {
        assert(strcmp(cmd->args[i], expected_args[i]) == 0);
        printf("Verified arg[%d]: %s\n", i, cmd->args[i]);
    }
    printf("✓ Verification passed\n");
}

int main() {
    struct Command *cmd;

    // Test 1: Basic command with output redirection
    char test1[] = "ls -l -a > output.txt";
    cmd = parser(test1);
    print_command(cmd, "Basic command with output redirection");
    char *expected_args1[] = {"ls", "-l", "-a"};
    verify_command(cmd, "ls", 3, expected_args1, "Test 1");
    assert(cmd->redirection->output_file != NULL);
    assert(strcmp(cmd->redirection->output_file, "output.txt") == 0);
    free_command(cmd);

    // Test 2: Command with multiple redirections
    char test2[] = "grep foo < input.txt > output.txt 2> error.txt";
    cmd = parser(test2);
    print_command(cmd, "Command with multiple redirections");
    char *expected_args2[] = {"grep", "foo"};
    verify_command(cmd, "grep", 2, expected_args2, "Test 2");
    free_command(cmd);

    // Test 3: Command with append redirection
    char test3[] = "cat -n input.txt >> output.txt";
    cmd = parser(test3);
    print_command(cmd, "Command with append redirection");
    char *expected_args3[] = {"cat", "-n", "input.txt"};
    verify_command(cmd, "cat", 3, expected_args3, "Test 3");
    free_command(cmd);

    printf("\nAll tests passed successfully!\n");
    return 0;
}
