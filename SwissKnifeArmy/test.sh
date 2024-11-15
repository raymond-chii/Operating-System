#!/bin/bash

# Colors for better readability
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Function to cleanup processes and files
cleanup() {
    # Kill any remaining catgrepmore, grep, or more processes
    pkill -f "catgrepmore"
    pkill -f "grep FIND_THIS_PATTERN"
    pkill -f "more"
    
    # Remove test files
    rm -f file1.txt file2.txt file3.txt large_file.txt
    
    # Reset terminal in case more left it in a bad state
    stty sane
}

# Set up trap to ensure cleanup on script exit
trap cleanup EXIT INT TERM

echo "Starting catgrepmore tests..."

# Create test files
echo "Creating test files..."

# Create a large file (>64K) for testing pipe buffers
dd if=/dev/urandom bs=1024 count=100 2>/dev/null | base64 > large_file.txt
# Add some known patterns
echo "FIND_THIS_PATTERN: Test line 1" >> large_file.txt
echo "Random content" >> large_file.txt
echo "FIND_THIS_PATTERN: Test line 2" >> large_file.txt

# Create multiple smaller files for testing multi-file functionality
echo "FIND_THIS_PATTERN: File 1 content" > file1.txt
echo "Random content in file 1" >> file1.txt

echo "FIND_THIS_PATTERN: File 2 content" > file2.txt
echo "Random content in file 2" >> file2.txt

echo "FIND_THIS_PATTERN: File 3 content" > file3.txt
echo "Random content in file 3" >> file3.txt

# Compile the program
echo "Compiling catgrepmore..."
gcc -o catgrepmore catgrepmore.c
if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed${NC}"
    exit 1
fi
echo -e "${GREEN}Compilation successful${NC}"

# Function to run a test with timeout
run_test() {
    local test_name=$1
    local cmd=$2
    local timeout_duration=${3:-5}
    
    echo -e "\n$test_name"
    
    # Run the command in background
    eval "$cmd" &
    local PID=$!
    
    # Wait for specified duration
    sleep 1
    
    # Send quit signal
    kill -QUIT $PID 2>/dev/null
    
    # Wait for process to finish with timeout
    local timeout=$timeout_duration
    while kill -0 $PID 2>/dev/null && [ $timeout -gt 0 ]; do
        sleep 1
        ((timeout--))
    done
    
    # Force kill if still running
    if kill -0 $PID 2>/dev/null; then
        kill -9 $PID 2>/dev/null
        wait $PID 2>/dev/null
        echo -e "${RED}Test timed out${NC}"
        return 1
    fi
    
    wait $PID 2>/dev/null
    echo -e "${GREEN}$test_name completed${NC}"
    return 0
}

# Test 1: Basic functionality
run_test "Test 1: Basic functionality" \
    "./catgrepmore 'FIND_THIS_PATTERN' file1.txt"

# Test 2: Multiple files
run_test "Test 2: Multiple files" \
    "./catgrepmore 'FIND_THIS_PATTERN' file1.txt file2.txt file3.txt"

# Test 3: Large file handling
run_test "Test 3: Large file handling" \
    "./catgrepmore 'FIND_THIS_PATTERN' large_file.txt"

# Test 4: SIGUSR1 signal handling (status display)
echo -e "\nTest 4: SIGUSR1 signal handling"
./catgrepmore "FIND_THIS_PATTERN" large_file.txt &
PID=$!
sleep 1
kill -SIGUSR1 $PID
sleep 1
kill -QUIT $PID 2>/dev/null
wait $PID 2>/dev/null
echo -e "${GREEN}Test 4 completed${NC}"

# Test 5: SIGUSR2 signal handling (skip to next file)
echo -e "\nTest 5: SIGUSR2 signal handling"
./catgrepmore "FIND_THIS_PATTERN" large_file.txt file1.txt file2.txt &
PID=$!
sleep 1
kill -SIGUSR2 $PID
sleep 1
kill -QUIT $PID 2>/dev/null
wait $PID 2>/dev/null
echo -e "${GREEN}Test 5 completed${NC}"

# Test 6: Non-existent file handling
echo -e "\nTest 6: Non-existent file handling"
./catgrepmore "FIND_THIS_PATTERN" nonexistent.txt 2>&1 | grep "open"
if [ $? -eq 0 ]; then
    echo -e "${GREEN}Error properly reported${NC}"
else
    echo -e "${RED}Error handling failed${NC}"
fi

# Test 7: Process cleanup verification
echo -e "\nTest 7: Process cleanup verification"
./catgrepmore "FIND_THIS_PATTERN" large_file.txt &
PID=$!
sleep 1
kill -QUIT $PID 2>/dev/null
sleep 2

# Check for any lingering processes
LINGERING=$(ps aux | grep -E "grep FIND_THIS_PATTERN|more" | grep -v "grep -E")
if [ -z "$LINGERING" ]; then
    echo -e "${GREEN}No lingering processes found${NC}"
else
    echo -e "${RED}Found lingering processes:${NC}"
    echo "$LINGERING"
    # Cleanup lingering processes
    pkill -f "grep FIND_THIS_PATTERN"
    pkill -f "more"
fi

echo -e "\n${GREEN}All tests completed${NC}"