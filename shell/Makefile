CC = gcc
CFLAGS = -Wall -Werror

main: parser.o builtin.o utils.o executor.o
	$(CC) $(CXXFLAGS) -o main main.o parser.o builtin.o utils.o executor.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

builtin.o: builtin.c builtin.h
	$(CC) $(CFLAGS) -c builtin.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

executor: executor.c executor.h
	$(CC) $(CFLAGS) -c executor.c

clean:
	rm -f main *.o *.stackdump *~
