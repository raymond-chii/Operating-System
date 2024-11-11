#!./main

# Test that comments are handled properly
# Create test directory
cd /tmp
pwd

# Test basic output redirection
echo "Hello World" >hello.txt
cat hello.txt
ls -l hello.txt

# Test append redirection
echo "Second line" >>hello.txt
cat hello.txt

# Test input redirection
cat <hello.txt >hello2.txt
ls -l hello2.txt

# Test error redirection
ls nonexistent_file 2>error.txt
cat error.txt

# Test exit status variable
false
echo "Previous exit status: $?"
true
echo "Previous exit status: $?"

# Test handling of nonexistent command
nonexistentcommand
echo "Exit status of nonexistent command: $?"

# Test permission denied scenario
touch noperm.txt
chmod 000 noperm.txt
echo "test" >noperm.txt
ls -l noperm.txt

# Test cd back to home directory
cd
pwd

# Clean up test files
cd /tmp
rm -f hello.txt hello2.txt error.txt noperm.txt

# Exit with a specific status to verify exit handling
exit 42