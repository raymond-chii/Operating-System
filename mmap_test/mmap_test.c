#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>



void signal_handler(int signum) {
    exit(signum);
}

int test1(){

// My enviroment is MacOS and test 1 returns 10 which is SIGBUS 
// but it should be SIGSEGV -- 
// "If the attempted access is a violation of the regions 
// protections, a signal SIGSEGV is forcibly posted to the current process."
//
// so I ran it in Docker and got it to return 11 which is SIGSEGV

    int fd = open("test.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1){
        perror("Error opening file");
        return 255;
    }
    write(fd, "A", 1);

    char *mapped = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {

        perror("mmap");
        return 255;
    
    }

    for(int i = 1; i <= 31; i++) {
        signal(i, signal_handler);
    }

    char initial = mapped[0];
    printf("Initial value: %c\n", initial);
    mapped[0] = 'H';
    printf("After write: %c\n", mapped[0]);

    munmap(mapped, 4096);
    close(fd);

    return (mapped[0] == 'H') ? 0 : 255;

}

int test2() {

// The test.txt had character "A" in it and we successfully write "B"
// to it. Also, traditional system calls such as read() and lseek(),
// were able to read "B". 
// 
// "When MAP_SHARED is specified, writing to the memory region is 
// immediately and transparently visible to this or any other process
// through the traditional system calls such as read."

    int fd = open("test.txt", O_RDWR | O_CREAT, 0666);
    write(fd, "A", 1);
    
    char *mapped = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("Before write: %c\n", mapped[0]);

    mapped[0] = 'B';

    lseek(fd, 0, SEEK_SET);
    char buf;
    read(fd, &buf, 1);
    printf("After read: %c\n", buf);

    close(fd);
    munmap(mapped, 4096);

    return (buf == 'B') ? 0 : 1;
}

int test3(){

// The test.txt had character "A" in it and we failed to write "B" and returns 1.
// 
// "When the mapping is MAP_PRIV ATE, Copy on Write (see below) happens. Upon the
// first memory write access to a page in a MAP_PRIV ATE region, the association between
// that virtual page and the file is broken. Thus writes to a MAP_PRIV ATE region do NOT
// cause the associated file to be modified." 

    int fd = open("test.txt", O_RDWR | O_CREAT, 0666);
    write(fd, "A", 1);
    
    char *mapped = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("Before write: %c\n", mapped[0]);

    mapped[0] = 'B';

    lseek(fd, 0, SEEK_SET);
    char buf;
    read(fd, &buf, 1);
    printf("After read: %c\n", buf);

    close(fd);
    munmap(mapped, 4096);

    return (buf == 'B') ? 0 : 1;
}

int test4() {

    int fd = open("test.txt", O_RDWR | O_CREAT, 0666);
    for (int i = 0; i < 4101; i++) {
        write(fd, "A", 1);
    }
    close(fd);

    fd = open("test.txt", O_RDWR);
    char *mapped = mmap(NULL, 8192, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

// reading back from 4101 to 8192 and it returns 0

    for (int i = 4101; i < 8192; i++) {
        if (mapped[i] != 0) {
            printf("Error: byte at offset %d is not 0\n", i);
            return 1;
        }
    }

    mapped[4101] = 'X';

// creating "hole" between 4101 and 4116

    lseek(fd, 4116, SEEK_SET);
    write(fd, "Y", 1);

// checked X and its visible
    char buf[1];
    lseek(fd, 4101, SEEK_SET);
    read(fd, buf, 1);

    return (buf[0] == 'X') ? 0 : 1;
    
}

int main(int argc, char **argv){

    if (argc > 2){
        fprintf(stderr, "Usage: %s test_number\n", argv[0]);
        return 1;
    }

    int test_number = atoi(argv[1]);

    switch(test_number){
        case 1:
            printf("Test 1\n");
            return test1();
        case 2:
            printf("Test 2\n");
            return test2();
        case 3:
            printf("Test 3\n");
            return test3();
        case 4:
            printf("Test 4\n");
            return test4();
        default:
            fprintf(stderr, "Invalid test number\n");
            return 1;
    }
    return 0;
}