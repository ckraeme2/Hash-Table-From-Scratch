/* duplicates.c */

#include "hash.h"
#include "macros.h"
#include "table.h"

#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>

/* Globals */

char * PROGRAM_NAME = NULL;

/* Structures */

typedef struct {
    bool count;
    bool quiet;
} Options;

/* Functions */

void usage(int status) {
    fprintf(stderr, "Usage: %s paths...\n", PROGRAM_NAME);
    fprintf(stderr, "    -c     Only display total number of duplicates\n");
    fprintf(stderr, "    -q     Do not write anything (exit with 0 if duplicate found)\n");
    exit(status);
}

/**
 * Check if path is a directory.
 * @param       path        Path to check.
 * @return      true if Path is a directory, otherwise false.
 */
bool is_directory(const char *path) {
    
    struct stat path_stat;
    stat(path, &path_stat);

    if(S_ISDIR(path_stat.st_mode) == 1){
        return true;
    }
    else{
        return false;
    }

}

/**
 * Check if file is in table of checksums.
 *
 *  If quiet is true, then exit if file is in checksums table.
 *
 *  If count is false, then print duplicate association if file is in
 *  checksums table.
 *
 * @param       path        Path to file to check.
 * @param       checksums   Table of checksums.
 * @param       options     Options.
 * @return      0 if Path is not in checksums, otherwise 1.
 */
size_t check_file(const char *path, Table *checksums, Options *options) {
    char digest[HEX_DIGEST_LENGTH];
    Value val;
    if(hash_from_file(path, digest)){    
        if(table_search(checksums, digest) && options->quiet){
            exit(0);
        }
        else if(table_search(checksums, digest) && !(options->count)){
            printf("%s is a duplicate of %s\n", path, table_search(checksums,digest)->string);
        }
        else if(table_search(checksums, digest)){
            return 1;
        }
        val.string = (char *)path;
        table_insert(checksums, digest, val, STRING);
        return 0;
    }

    return 0;
}
/**
 * Check all entries in directory (recursively).
 * @param       root        Path to directory to check.
 * @param       checksums   Table of checksums.
 * @param       options     Options.
 * @return      Number of duplicate entries in directory.
 */
size_t check_directory(const char *root, Table *checksums, Options *options) {
    DIR *d = opendir(root);
    char buf[BUFSIZ];
    size_t num = 0;
    if(!d){
        return EXIT_FAILURE;
    }

    for(struct dirent *e = readdir(d); e; e= readdir(d)){
        if(streq(e->d_name, ".") || streq(e->d_name, "..")){
            continue;
        }
        sprintf(buf, "%s/%s", root, e->d_name);
        if(is_directory(buf)){
            num += check_directory(buf, checksums, options);
        }
        else{
            num += check_file(buf, checksums, options);
        }
    }

    closedir(d);

    return num;
}

/* Main Execution */

int main(int argc, char *argv[]) {
    /* Parse command line arguments */
   if(argc < 2){
     usage(0);
   }

    argv++;
    Options base;
    base.quiet = false;
    base.count = false;
    Table *checksums = table_create(0);
    size_t num = 0;
    while( --argc > 0 ){
        if(argv[0][0] == '-'){
            if(argv[0][1] == 'q'){
                base.quiet = true;
            }
            else if(argv[0][1] == 'c'){
                puts("c on");
                base.count = true;
            }
            else if(argv[0][1] == 'h'){
                usage(0);
            }
            else{
                if(is_directory(argv[0])){
                    num += check_directory(argv[0], checksums, &base);
                }
                else{
                    num += check_file(argv[0], checksums, &base);
                }
            }
        }
        else{
            if(is_directory(argv[0])){
                num += check_directory(argv[0], checksums, &base);
            }
            else{
                num += check_file(argv[0], checksums, &base);
            }
        }

        argv++;
    }
                


    /* Check each argument */

    /* Display count if necessary */
    if(base.count == true){
        printf("%zu\n", num);
    }
    
    
    table_delete(checksums);
    
    if(base.quiet == true){
       return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
