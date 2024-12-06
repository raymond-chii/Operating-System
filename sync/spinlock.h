#ifndef __SPINLOCK_H
#define __SPINLOCK_H

struct spinlock {
    volatile char lock;
};

void spin_lock(struct spinlock *l);
void spin_unlock(struct spinlock *l);

#endif
