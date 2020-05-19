/* hash.c: hash functions */

#include "hash.h"
#include <openssl/md5.h>
#include <stdio.h>

/**
 * Constants
 * http://isthe.com/chongo/tech/comp/fnv/
 */

#define FNV_OFFSET_BASIS    (0xcbf29ce484222325ULL)
#define FNV_PRIME           (0x100000001b3ULL)

/**
 * Compute FNV-1 hash.
 * @param   data            Data to hash.
 * @param   n               Number of bytes in data.
 * @return  Computed hash as 64-bit unsigned integer.
 */
uint64_t    hash_from_data(const void *data, size_t n) {
    uint64_t hash = FNV_OFFSET_BASIS;
    uint8_t *ptr = (uint8_t *)data;
    for(size_t i = 0; i <= n-1; i++){
        hash ^= *ptr;
        hash = hash * FNV_PRIME;
        ptr++;
    }
    
    return hash;
}

/**
 * Compute MD5 digest.
 * @param   path            Path to file to checksum.
 * @param   hexdigest       Where to store the computed checksum in hexadecimal.
 * @return  Whether or not the computation was successful.
 */
bool        hash_from_file(const char *path, char hexdigest[HEX_DIGEST_LENGTH]) {
    MD5_CTX c;
    char buf[BUFSIZ];
    ssize_t bytes;
    FILE * file;
    file = fopen(path, "r");
    unsigned char out[MD5_DIGEST_LENGTH];
    
    if(file == NULL){
        return false;
    }

    MD5_Init(&c);
    bytes = fread(buf, sizeof(char), BUFSIZ, file);
    
    while(bytes > 0){
        MD5_Update(&c, buf, bytes);
        bytes = fread(buf, sizeof(char), BUFSIZ, file);
    }
    
    MD5_Final(out, &c);

    for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
        sprintf(hexdigest+i*2, "%02x", out[i]);
    }
    
    hexdigest[2*MD5_DIGEST_LENGTH] = '\0';    
    return true;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
