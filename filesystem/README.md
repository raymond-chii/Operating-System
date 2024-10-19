# Recursive Filesystem Statistics Checker

This program recursively explores the filesystem starting from a given directory and tabulates various statistics about the files and directories encountered.

## Features

The program reports the following statistics:

1. Number of inodes of each type (e.g., directory, file, symlink, etc.)
2. Total size and number of disk blocks allocated for regular files
3. Number of hard-linked inodes (excluding directories)
4. Number of unresolved symlinks
5. Number of pathnames that would be "problematic" to enter at the keyboard

## Compilation

To compile the program, run:

```
make
```

This will create an executable named `filesystem`.

## Usage

Run the program with a single argument specifying the directory to explore:

```
./filesystem <directory>
```

For example:

```
./filesystem /home/user
```

## Output

The program will print information about each file or directory it encounters, followed by a summary of statistics at the end.

## Error Handling

The program attempts to handle errors gracefully. If it encounters a part of the filesystem where it doesn't have permission, it will report the error and continue exploring other accessible areas.

## Files

- `filesystem.c`: Main source file containing the program logic
- `filesystem.h`: Header file with function prototypes and structure definitions
- `Makefile`: For compiling the program

## Testing

A test script `simulation.sh` is provided to run various tests on the program. To run the tests:

```
./simulation.sh
```

This will create a file `submission_test_results.txt` containing the test results.

## Notes

- The program calculates spatial efficiency as the ratio of total file size to total allocated blocks.
- "Problematic" names include those with non-printable characters, non-ASCII characters, or special characters that could confuse the shell.
- The program does not follow symbolic links to avoid potential infinite loops.
