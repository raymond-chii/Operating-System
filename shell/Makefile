CC = gcc
CFLAGS = -Wall -Werror

main: main.o parser.o builtins.o executor.o
	$(CC) $(CXXFLAGS) -o main main.o parser.o builtins.o executor.o

main.o: main.c parser.h executor.h builtins.h
	$(CC) $(CFLAGS) -c main.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

builtins.o: builtins.c builtins.h
	$(CC) $(CFLAGS) -c builtins.c

executor.o: executor.c executor.h
	$(CC) $(CFLAGS) -c executor.c

clean:
	rm -f main *.o *.stackdump *~
