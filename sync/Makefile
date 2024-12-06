CC = gcc
CFLAGS = -Wall -g

all: shellgame

shellgame: shellgame.o sem.o spinlock.o tas64.o
	$(CC) $(CFLAGS) -o shellgame shellgame.o sem.o spinlock.o tas64.o

shellgame.o: shellgame.c sem.h spinlock.h
	$(CC) $(CFLAGS) -c shellgame.c

sem.o: sem.c sem.h spinlock.h
	$(CC) $(CFLAGS) -c sem.c

spinlock.o: spinlock.c spinlock.h tas.h
	$(CC) $(CFLAGS) -c spinlock.c

tas64.o: tas64.S
	$(CC) $(CFLAGS) -c tas64.S

clean:
	rm -f *.o shellgame

.PHONY: all clean