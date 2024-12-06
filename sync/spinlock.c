#include <sched.h>
#include "tas.h"

struct spinlock {
    volatile char lock;
};

void spin_lock(struct spinlock *l) {
    while (tas(&l->lock)) {
        sched_yield();
    }
}

void spin_unlock(struct spinlock *l) {
    l->lock = 0;
}
