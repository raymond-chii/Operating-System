#!/bin/bash

# Redirect output to file
exec > filesystem_test_results.txt 2>&1

# Function to run test and print result
run_test() {
    echo "Testing directory: $1"
    echo "$ ./filesystem \"$1\""
    ./filesystem "$1"
    echo -e "\n---\n"
}

# Compile the program
echo "$ make"
make
echo

# Create test directories and files
mkdir -p test_dirs/normal_dir
echo "This is file1" > test_dirs/normal_dir/file1.txt
echo "This is file2" > test_dirs/normal_dir/file2.txt
touch test_dirs/normal_dir/empty_file

mkdir -p test_dirs/empty_dir

mkdir -p test_dirs/symlink_dir
ln -s ../normal_dir/file1.txt test_dirs/symlink_dir/working_symlink
ln -s nonexistent_file test_dirs/symlink_dir/broken_symlink

mkdir -p test_dirs/hardlink_dir
echo "Hardlinked file" > test_dirs/hardlink_dir/hardlink_file
ln test_dirs/hardlink_dir/hardlink_file test_dirs/hardlink_dir/hardlink_file2

mkdir -p "test_dirs/problematic dir"
echo "File with spaces" > "test_dirs/problematic dir/file with spaces.txt"
echo "Non-ASCII filename" > "test_dirs/problematic dir/file_with_非ascii.txt"

# Run tests
run_test "test_dirs/normal_dir"
run_test "test_dirs/empty_dir"
run_test "test_dirs/symlink_dir"
run_test "test_dirs/hardlink_dir"
run_test "test_dirs/problematic dir"

# Test with a system directory (this may require root privileges)
run_test "/etc"

# Clean up
echo "$ rm -rf test_dirs"
rm -rf test_dirs

echo "$ make clean"
make clean

echo "All test results have been saved to filesystem_test_results.txt"