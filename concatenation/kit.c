#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int output_fd; 

void write_file(const char *buffer, int size) {
    
    ssize_t bytes_written = 0;
    
    while (bytes_written < size){
        ssize_t result = write (output_fd, buffer + bytes_written, size - bytes_written);
        if (result == -1) {
            perror("Error writing to output file");
            exit(-1);
        }
        bytes_written += result;
    }
}

void handle_input(const char *filename){

    int fd;
    char buf[4096];
    ssize_t bytes_read;

    if (strcmp(filename, "-") == 0) {
        fd = STDIN_FILENO;
    } else{
        fd = open(filename, O_RDONLY);
        
        if (fd == -1) {
            perror("Error opening input file");
            fprintf(stderr, "%s\n", filename); // Comments: include the filename in the error message
            exit(-1);
        }
    }
    while ((bytes_read = read(fd, buf, sizeof(buf))) > 0) {
        write_file(buf, bytes_read);
    }

    if (bytes_read == -1) {
        perror("Error reading input file");
        exit(-1);
    }

    if (fd != STDIN_FILENO) {
        if (close(fd) == -1){
            perror("Error closing input file");
            exit(-1);
        }
        
    }
}


int main(int argc, char *argv[]) {

    int opt;
    char *outfile = NULL;

    while((opt = getopt(argc, argv, "o:")) != -1) {
        switch (opt) {
        case 'o':
            outfile = optarg;
            break;
        case '?':
            fprintf(stderr, "Usage: %s [-o outfile] [infile1 [infile2 ...]]\n", argv[0]);
            fprintf(stderr, "       %s [-o outfile]\n", argv[0]);
            exit(-1);
        }
    }

    if (outfile != NULL) {
        output_fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666); 
        // Comments: use 0666 instead of 0644 to ensure that the file is created with the correct permissions
        if (output_fd == -1) {
            perror("Error opening output file");
            return -1;
        }
    }else {
        output_fd = STDOUT_FILENO;
    }

    if (optind < argc) { 
        for (int i = optind; i < argc; i++) {
            handle_input(argv[i]);
        }
    } else { 
        handle_input("-");
    }

    if (output_fd != STDOUT_FILENO) {
        if (close(output_fd) == -1) {
            perror("Error closing output file");
            exit(-1);
        }
    }

    close(output_fd);

    return 0;
}
