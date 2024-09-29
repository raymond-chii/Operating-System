#!/bin/bash

# Redirect all output to a file
exec > kit_test_results.txt 2>&1
make

# Create test files
echo "This is file1" > file1.txt
echo "This is file2" > file2.txt
echo "" > empty.txt
dd if=/dev/urandom of=binary_file bs=1024 count=100 2>/dev/null

# Function to run test and print result
run_test() {
    echo "Test: $1"
    echo "Command: $2"
    eval $2
    echo "Exit status: $?"
    echo "-------------------"
}

# Test cases
run_test "Basic file concatenation" "./kit file1.txt file2.txt"
run_test "Using standard input with files" "echo 'Standard input' | ./kit file1.txt - file2.txt"
run_test "Multiple instances of standard input" "(echo 'Input 1'; echo 'Input 2') | ./kit - - file1.txt"
run_test "Output to a file" "./kit -o output.txt file1.txt file2.txt && cat output.txt"
run_test "Reading only from standard input" "echo 'Only stdin' | ./kit"
run_test "Using output file with standard input" "echo 'Stdin to file' | ./kit -o output2.txt - && cat output2.txt"
run_test "Binary file handling" "./kit binary_file | sha1sum"
run_test "Error handling for non-existent file" "./kit nonexistent_file.txt"
run_test "Mixing existing and non-existing files" "./kit file1.txt nonexistent_file.txt file2.txt"
run_test "Using a single hyphen as a filename" "echo '- file' > ./- && ./kit ./-"
run_test "Large file handling" "./kit input.txt | wc -c"
run_test "Empty file handling" "./kit empty.txt file1.txt"

# Clean up
rm file1.txt file2.txt empty.txt binary_file output.txt output2.txt ./-
