#!/bin/bash

make

# Function to log commands and their output
log_command() {
    echo "$ $1" >> test_results.txt
    eval "$1" >> test_results.txt 2>&1
    echo "" >> test_results.txt
}

# Function to run the program and check output
run_test() {
    echo "Testing: $2" >> test_results.txt
    output=$(./filesystem "$1" 2>&1)
    echo "$output" >> test_results.txt
    
    # Check for expected values
    for check in "${@:3}"; do
        if echo "$output" | grep -q "$check"; then
            echo "PASS: $check" >> test_results.txt
        else
            echo "FAIL: Expected $check" >> test_results.txt
        fi
    done
    
    echo -e "\n---\n" >> test_results.txt
}

# Compile the program
log_command "make"

# Create test directories
log_command "mkdir -p test_dirs/normal_dir test_dirs/empty_dir test_dirs/symlink_dir test_dirs/hardlink_dir \"test_dirs/problematic dir\""

# Create test files
log_command "dd if=/dev/zero of=test_dirs/normal_dir/file_1MB bs=1M count=1"
log_command "echo \"Test file\" > test_dirs/normal_dir/file2.txt"
log_command "touch test_dirs/normal_dir/empty_file"

# Create symlinks
log_command "ln -s ../normal_dir/file_1MB test_dirs/symlink_dir/working_symlink"
log_command "ln -s ../non_existent_file test_dirs/symlink_dir/broken_symlink"

# Create hard links
log_command "ln test_dirs/normal_dir/file2.txt test_dirs/hardlink_dir/hardlink_file"

# Create files with problematic names
log_command "touch \"test_dirs/problematic dir/file with spaces.txt\""
log_command "touch \"test_dirs/problematic dir/file_with_非ascii.txt\""

# Create a FIFO
log_command "mkfifo test_dirs/normal_dir/test_fifo"

# Create the results file
echo "Filesystem Statistics Checker Comprehensive Test Results" > test_results.txt
echo "=======================================================" >> test_results.txt
echo "" >> test_results.txt

# Run tests
run_test "test_dirs/normal_dir" "Normal directory" \
    "Number of regular files: 3" \
    "Number of directories: 1" \
    "Number of symlinks: 0" \
    "Number of fifos: 1" \
    "Total size: 1048581" \
    "Hard linked: 1" \
    "Problematic names: 0"

run_test "test_dirs/empty_dir" "Empty directory" \
    "Number of regular files: 0" \
    "Number of directories: 1" \
    "Total size: 0"

run_test "test_dirs/symlink_dir" "Directory with symlinks" \
    "Number of symlinks: 2" \
    "Unresolved symlinks: 1"

run_test "test_dirs/hardlink_dir" "Directory with hard links" \
    "Number of regular files: 1" \
    "Hard linked: 1"

run_test "test_dirs/problematic dir" "Directory with problematic names" \
    "Number of regular files: 2" \
    "Problematic names: 3"  # 2 files + 1 directory name

# Test error handling (assuming /root is not accessible)
run_test "/root" "Inaccessible directory" \
    "Errors: 1"

# Clean up
log_command "rm -rf test_dirs"
log_command "make clean"

echo "Testing complete. Results are in test_results.txt"

make clean


