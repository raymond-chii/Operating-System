#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "sem.h"
#include <unistd.h>
#include <signal.h>

#define NUM_TASKS 6

struct shared_data {
    struct sem sem_A;
    struct sem sem_B;
    struct sem sem_C;
    int signal_handler_calls[NUM_TASKS];
};

// Make shared data accessible to signal handler
static struct shared_data *shared_ptr;

static void sigusr1_handler(int sig) {
    shared_ptr->signal_handler_calls[my_procnum]++;
}

void make_moves(struct shared_data *shared, int task_num, int num_moves) {
    // Set up signal handler
    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);
    
    switch(task_num) {
        case 0: // Move A -> B
            for(int i = 0; i < num_moves; i++) {
                sem_wait(&shared->sem_A);
                sem_inc(&shared->sem_B);
            }
            break;
        case 1: // Move A -> C
            for(int i = 0; i < num_moves; i++) {
                sem_wait(&shared->sem_A);
                sem_inc(&shared->sem_C);
            }
            break;
        case 2: // Move B -> A
            for(int i = 0; i < num_moves; i++) {
                sem_wait(&shared->sem_B);
                sem_inc(&shared->sem_A);
            }
            break;
        case 3: // Move B -> C
            for(int i = 0; i < num_moves; i++) {
                sem_wait(&shared->sem_B);
                sem_inc(&shared->sem_C);
            }
            break;
        case 4: // Move C -> A
            for(int i = 0; i < num_moves; i++) {
                sem_wait(&shared->sem_C);
                sem_inc(&shared->sem_A);
            }
            break;
        case 5: // Move C -> B
            for(int i = 0; i < num_moves; i++) {
                sem_wait(&shared->sem_C);
                sem_inc(&shared->sem_B);
            }
            break;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s initial_count num_moves\n", argv[0]);
        return 1;
    }

    int initial_count = atoi(argv[1]);
    int num_moves = atoi(argv[2]);

    struct shared_data *shared = mmap(NULL, 
                                    sizeof(struct shared_data),
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED | MAP_ANONYMOUS,
                                    -1, 0);

    if (shared == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    shared_ptr = shared;

    sem_init(&shared->sem_A, initial_count);
    sem_init(&shared->sem_B, initial_count);
    sem_init(&shared->sem_C, initial_count);

    for(int i = 0; i < NUM_TASKS; i++) {
        shared->signal_handler_calls[i] = 0;
    }

    for(int i = 0; i < NUM_TASKS; i++) {
        if(fork() == 0) {
            // Child process
            my_procnum = i;
            printf("VCPU %d starting, pid %d\n", i, getpid());
            
            make_moves(shared, i, num_moves);
            
            printf("Child %d (pid %d) done, signal handler was invoked %d times\n",
                   i, getpid(), shared->signal_handler_calls[i]);
            printf("VCPU %d done\n", i);
            exit(0);
        }
    }

    printf("Main process spawned all children, waiting\n");

    while(wait(NULL) > 0);

    printf("All children done!\n");
    printf("Sem# val Sleeps Wakes\n");
    
    printf("Semaphore A count=%d\n", shared->sem_A.count);
    for(int i = 0; i < NUM_TASKS; i++) {
        printf("VCPU %d slept %d woke %d\n", 
               i,
               shared->sem_A.sleeping_count[i],
               shared->sem_A.wake_count[i]);
    }

    printf("\nSemaphore B count=%d\n", shared->sem_B.count);
    for(int i = 0; i < NUM_TASKS; i++) {
        printf("VCPU %d slept %d woke %d\n", 
               i,
               shared->sem_B.sleeping_count[i],
               shared->sem_B.wake_count[i]);
    }

    printf("\nSemaphore C count=%d\n", shared->sem_C.count);
    for(int i = 0; i < NUM_TASKS; i++) {
        printf("VCPU %d slept %d woke %d\n", 
               i,
               shared->sem_C.sleeping_count[i],
               shared->sem_C.wake_count[i]);
    }

    munmap(shared, sizeof(struct shared_data));
    return 0;
}