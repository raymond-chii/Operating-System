#include "spinlock.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct shared_space{
    long counter;
    struct spinlock lock;
};

#define N 1000000
#define NUM_PROCESSES 8

int main(){

    struct shared_space *shared = mmap(NULL, 
                                sizeof(struct shared_space),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS,
                                -1, 0);

    if (shared == MAP_FAILED){
        perror("mmap");
        exit(1);
    }

    shared->counter = 0;
    shared->lock.lock = 0;

    printf("Without Protection\n");
    for (int i = 0; i < NUM_PROCESSES; i++){
        if (fork() == 0){
            for (int j = 0; j < N; j++){
                shared->counter++;
            }
            exit(0);
        }
    }

    while (wait(NULL) > 0);
    
    printf("Expected: %ld\n", (long)NUM_PROCESSES * N);
    printf("Actual: %ld\n\n", shared->counter);

    shared->counter = 0;

    printf("With Protection\n");
    for (int i = 0; i < NUM_PROCESSES; i++){
        if (fork() == 0){
            for (int j = 0; j < N; j++){

                // critical section
                spin_lock(&shared->lock);
                shared->counter++;
                spin_unlock(&shared->lock);
                // critical section

            }
            exit(0);
        }
    }

    while (wait(NULL) > 0);
    
    printf("Expected: %ld\n", (long)NUM_PROCESSES * N);
    printf("Actual: %ld\n", shared->counter);

    munmap(shared, sizeof(struct shared_space));
    return 0;

}