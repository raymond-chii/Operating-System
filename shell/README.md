# shell program

## Description

This is a custom shell implementation that provides basic command-line functionality similar to standard Unix shells. It supports built-in commands, file redirections, and process execution with detailed statistics.

## Features

- Built-in commands:
  - `cd`: Change directory with support for HOME directory
  - `pwd`: Print working directory
  - `exit`: Exit the shell with optional status code

- File redirections:
  - Input redirection (`<`)
  - Output redirection (`>`)
  - Output append redirection (`>>`)
  - Error redirection (`2>`)
  - Error append redirection (`2>>`)

- Process execution:
  - Executes external commands from PATH
  - Displays process statistics including:
    - Real time
    - User time
    - System time
    - Exit status
    - Signal information (if terminated by signal)

- Variable expansion:
  - `$?`: Expands to the exit status of the last command

- Error handling:
  - Detailed error messages for common scenarios
  - Proper handling of file permissions
  - Command not found handling

## Usage

```bash
# Run the shell interactively
./shell

# Execute commands from a file
./shell script.sh
```

## Building

To compile the program:

```bash
make
```

This will create the `shell` executable.

## Examples

1. Basic command execution:
```bash
$ ls -l
$ pwd
```

2. Using redirections:
```bash
$ ls > output.txt
$ grep error < input.txt > results.txt 2> errors.txt
```

3. Built-in commands:
```bash
$ cd /path/to/directory
$ pwd
$ exit 0
```

4. Using variable expansion:
```bash
$ command
$ echo $?  # prints the exit status of 'command'
```

## Testing

The project includes a test suite in `test_parser.c`. To run the tests:

```bash
make test
./test_parser
```

## Implementation Details

The shell is implemented with several key components:

- `parser.c`: Handles command parsing and tokenization
- `executor.c`: Manages command execution and process statistics
- `builtins.c`: Implements built-in shell commands
- `main.c`: Core shell loop and initialization

## Limitations

- No support for pipes (`|`)
- No support for command chaining (`;`, `&&`, `||`)
- No command history
- No tab completion
- Single command execution (no background processes)