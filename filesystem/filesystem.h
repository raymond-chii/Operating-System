#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <sys/types.h>

struct FileStats {
    int regular_files;
    int directories;
    int symlinks;
    int block_devices;
    int char_devices;
    int fifos;
    int sockets;
    int unknown;
    long long total_size;
    long total_blocks;
    int hard_linked;
    int unresolved_symlinks;
    int problematic_names;
    int error_count;
};

// Function prototypes
void explore_directory(const char *path);
void file_stats(const char *path);
char *inode_type(mode_t mode);
int is_problematic_name(const char *name);
int is_symlink_resolvable(const char *path);
void print_statistics();

#endif // FILESYSTEM_H