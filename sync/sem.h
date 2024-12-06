#include "spinlock.h"
#include <sys/types.h>

#define N_PROCS 64

volatile struct sem{
    int count;
    struct spinlock lock;
    pid_t waiting_pids[N_PROCS];
    int head;
    int tail;
    int vcpu_map[N_PROCS];
    int num_waiting;
    int sleeping_count[N_PROCS];
    int wake_count[N_PROCS];
};

extern int my_procnum; 

void sem_init(struct sem *s, int count);
// Initialize the semaphore *s with the initial count. Initialize
// any underlying data structures. sem_init should only be called
// once in the program (per semaphore). If called after the
// semaphore has been used, results are unpredictable.
// Note that the return type is void so no errors are anticipated.
// The pointer s is assumed to point within an established
// area of shared memory. This function does not allocate it!
int sem_try(struct sem *s);
// Attempt to perform the "P" operation (atomically decrement
// the semaphore). If this operation would block, return 0,
// otherwise return 1.
void sem_wait(struct sem *s);
// Perform the P operation, blocking until successful. See below
// about how blocking and waking are to be implemented.
void sem_inc(struct sem *s);
// Perform the V operation. If any other tasks were sleeping
// on this semaphore, wake them by sending a SIGUSR1 to their
// process id (which is not the same as the virtual processor number).
// If there are multiple sleepers (this would happen if multiple
// virtual processors attempt the P operation while the count is <1)
