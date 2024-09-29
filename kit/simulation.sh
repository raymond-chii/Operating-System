#!/bin/bash

# Redirect output to file
exec > submission_test_results.txt 2>&1

# Function to run test and print result
run_test() {
    echo "$ $1"
    eval $1
    echo
}

# Compile the program
echo "$ make"
make
echo

# Create test files
echo "Creating test files..."
echo "This is file1" > file1.txt
echo "This is file2" > file2.txt
echo "" > empty.txt
dd if=/dev/urandom of=binary_file bs=1024 count=100 2>/dev/null
echo

# Run tests
run_test "./kit file1.txt file2.txt"
run_test "echo 'Standard input' | ./kit file1.txt - file2.txt"
run_test "(echo 'Input 1'; echo 'Input 2') | ./kit - - file1.txt"
run_test "./kit -o output.txt file1.txt file2.txt && cat output.txt"
run_test "echo 'Only stdin' | ./kit"
run_test "echo 'Stdin to file' | ./kit -o output2.txt - && cat output2.txt"
run_test "./kit binary_file | head -c 20; echo" # Display first 20 bytes of binary file
run_test "./kit input.txt | wc -c"
run_test "./kit empty.txt file1.txt"

# Test error handling
echo "Testing error handling..."
run_test "./kit nonexistent_file.txt"
run_test "./kit file1.txt nonexistent_file.txt file2.txt"
run_test "./kit /root/secret.txt" # Permission denied (might vary based on system)
run_test "./kit -o /root/output.txt file1.txt" # Permission denied for output (might vary based on system)
run_test "./kit -o '' file1.txt" # Invalid output file name
run_test "./kit -o" # Missing argument for -o option
run_test "./kit -x" # Invalid option
run_test "chmod 000 file1.txt && ./kit file1.txt; chmod 644 file1.txt" # Permission denied for reading


# Clean up
echo "Cleaning up test files..."
rm file1.txt file2.txt empty.txt binary_file output.txt output2.txt
echo

echo "All test results have been saved to submission_test_results.txt"