#include "filesystem.h"
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct FileStats stats = {0};

void print_statistics() {
    printf("Number of regular files: %d\n", stats.regular_files);
    printf("Number of directories: %d\n", stats.directories);
    printf("Number of symlinks: %d\n", stats.symlinks);
    printf("Number of block devices: %d\n", stats.block_devices);
    printf("Number of char devices: %d\n", stats.char_devices);
    printf("Number of fifos: %d\n", stats.fifos);
    printf("Number of sockets: %d\n", stats.sockets);
    printf("Number of unknown: %d\n", stats.unknown);
    printf("Total size: %lld\n", stats.total_size);
    printf("Total blocks: %ld\n", stats.total_blocks);
    if (stats.total_blocks > 0) {
        double efficiency = (double)stats.total_size / (stats.total_blocks * 512);
        printf("Spatial efficiency: %.2f\n", efficiency);
    }
    printf("Hard linked: %d\n", stats.hard_linked);
    printf("Unresolved symlinks: %d\n", stats.unresolved_symlinks);
    printf("Problematic names: %d\n", stats.problematic_names);
    printf("Errors: %d\n", stats.error_count);
}

int is_problematic_name(const char *name) {
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isprint(name[i]) || !isascii(name[i]) || isspace(name[i]) ||
            name[i] == '\\' || name[i] == '/' || name[i] == ':' ||
            name[i] == '*' || name[i] == '?' || name[i] == '"' || 
            name[i] == '<' || name[i] == '>' || name[i] == '|' ||
            name[i] == '&'){
            return 1;
        }    
    }
    return 0;
}

int is_symlink_resolvable(const char *path) {
    struct stat st;
    
    // Use lstat to get information about the symlink itself
    if (lstat(path, &st) != 0) {
        return 0;
    }
    
    if (!S_ISLNK(st.st_mode)) {
        return 1;
    }
    
    // Now try to stat the target of the symlink
    if (stat(path, &st) == 0) {
        return 1; 
    } else {
        return 0;
    }
}

void file_stats(const char *path) {
    struct stat st;
    if (lstat(path, &st) == -1) {
        if (errno == EACCES) {
            fprintf(stderr, "Permission denied: Cannot access %s\n", path);
        } else if (errno == ENOENT) {
            fprintf(stderr, "No such file or directory: %s\n", path);
        } else {
            fprintf(stderr, "Error accessing %s: %s\n", path, strerror(errno));
        }
        stats.error_count++;
        return;
    }

    switch (st.st_mode & S_IFMT) {
        case S_IFREG:
            stats.regular_files++;
            stats.total_size += st.st_size;
            stats.total_blocks += st.st_blocks;
            if (st.st_nlink > 1){
                stats.hard_linked++;
                printf("Hard linked file: %s, Links: %lu\n", path, (unsigned long)st.st_nlink);
            }
            printf("Regular file: %s, Size: %lld bytes\n", path, (long long)st.st_size);
            break;
        case S_IFDIR:
            stats.directories++;
            break;
        case S_IFLNK:
            stats.symlinks++;
            if (!is_symlink_resolvable(path)) stats.unresolved_symlinks++;
            break;
        case S_IFBLK:
            stats.block_devices++;
            break;
        case S_IFCHR:
            stats.char_devices++;
            break;
        case S_IFIFO:
            stats.fifos++;
            break;
        case S_IFSOCK:
            stats.sockets++;
            break;
        default:
            stats.unknown++;
            break;
    }

    char *name = strrchr(path, '/');
    name = name ? name + 1 : (char *)path;
    if (is_problematic_name(name)) {
        stats.problematic_names++;
        printf("Problematic name found: %s\n", path);
    }

    printf("%s: %s\n", path, inode_type(st.st_mode));
}

char *inode_type(mode_t mode) {
    switch (mode & S_IFMT) {
        case S_IFREG: return "regular file";
        case S_IFDIR: return "directory";
        case S_IFLNK: return "symbolic link";
        case S_IFBLK: return "block device";
        case S_IFCHR: return "character device";
        case S_IFIFO: return "FIFO/pipe";
        case S_IFSOCK: return "socket";
        default: return "unknown";
    }
}

void explore_directory(const char *path) {
    DIR *dirp = opendir(path);
    struct dirent *entry;

    if (dirp == NULL) {
        fprintf(stderr, "Cannot open directory %s: %s\n", path, strerror(errno));
        stats.error_count++;
        return;
    }

    // Count the directory itself
    // file_stats(path);

    errno = 0; 

    while ((entry = readdir(dirp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[1024];
        if (snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name) >= sizeof(full_path)) {
            fprintf(stderr, "Path too long: %s/%s\n", path, entry->d_name);
            stats.error_count++;
            continue;
        }

        file_stats(full_path);

        struct stat st;
        if (lstat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            explore_directory(full_path);
        }
    }

    if (errno != 0) {
        fprintf(stderr, "Cannot read directory %s: %s\n", path, strerror(errno));
        stats.error_count++;
    }

    closedir(dirp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    explore_directory(argv[1]);
    print_statistics();

    return 0;
}
