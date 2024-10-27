#include <stdio.h>

int main() {
    int *ptr = NULL;
    // Attempting to dereference a NULL pointer will cause SIGSEGV
    *ptr = 42;
    return 0;
}