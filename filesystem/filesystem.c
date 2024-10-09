#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

void explore_directory(const char *path);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    explore_directory(argv[1]);

    return 0;
}

void file_stats(const char *path) {
    
    struct stat st;
    if (stat(path, &st) == -1){
        fprintf(stderr,"Can not stat %s:%s\\n",path,strerror(errno));
        return;
    }

    printf("File: %s\n", path);
    printf("Inode: %llu\n",(unsigned long long) st.st_ino);

    printf("Size: %lld\n",(long long) st.st_size);
    printf("Blocks: %lld\n",(long long) st.st_blocks);

    printf("IO block: %ld\n",(long) st.st_blksize);
    
    printf("Links: %lld\n",(long long) st.st_nlink);
    printf("Mode: %lld\n",(long long) st.st_mode);
    printf("\n");

}


void explore_directory(const char *path) {

    DIR *dirp = opendir(path);
    struct dirent *entry;

    if (dirp == NULL) {
        fprintf(stderr,"Can not open directory %s:%s\\n",path,strerror(errno));
        return;
    }

    errno = 0; 

    while ((entry = readdir(dirp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        
        file_stats(full_path);
    }

    if (errno != 0) {
        fprintf(stderr,"Can not read directory %s:%s\\n",path,strerror(errno));
    }

    closedir(dirp);
}