#include "sem.h"
#include "spinlock.h"
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

int my_procnum;

static void sigusr1_handler(int sig) {
    return;
}

void sem_init(struct sem *s, int count) {
    s->count = count;
    s->lock.lock = 0;
    s->head = 0;
    s->tail = 0;
    s->num_waiting = 0;
    
    for(int i = 0; i < N_PROCS; i++) {
        s->waiting_pids[i] = 0;
        s->vcpu_map[i] = 0;
        s->sleeping_count[i] = 0;
        s->wake_count[i] = 0;
    }
}

int sem_try(struct sem *s) {
    int success = 0;
    spin_lock(&s->lock);
    if (s->count > 0) {
        s->count--;
        success = 1;
    }
    spin_unlock(&s->lock);
    return success;
}

void sem_wait(struct sem *s) {
    sigset_t mask, oldmask;
    
    // Only handle signal masking here
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    
    while (1) {
        if (sem_try(s)) {
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            return;
        }
        
        spin_lock(&s->lock);
        if (s->count > 0) {
            s->count--;
            spin_unlock(&s->lock);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            return;
        }
        
        s->waiting_pids[s->tail] = getpid();
        s->vcpu_map[s->tail] = my_procnum;
        s->tail = (s->tail + 1) % N_PROCS;
        s->num_waiting++;
        s->sleeping_count[my_procnum]++;
        
        spin_unlock(&s->lock);
        
        sigsuspend(&oldmask);
    }
}

void sem_inc(struct sem *s) {
    spin_lock(&s->lock);
    s->count++;
    
    printf("VCPU %d incrementing sem, current count=%d\n", my_procnum, s->count);
    
    if (s->num_waiting > 0 && s->count > 0) {
        pid_t pid = s->waiting_pids[s->head];
        int vcpu = s->vcpu_map[s->head];
        
        printf("VCPU %d waking up VCPU %d\n", my_procnum, vcpu);
        
        s->head = (s->head + 1) % N_PROCS;
        s->num_waiting--;
        s->wake_count[vcpu]++;
        
        kill(pid, SIGUSR1);
    }
    
    spin_unlock(&s->lock);
}