/* pair.c: Key/Value Pair */

#include "pair.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/**
 * Create Pair structure.
 * @param   key             Pair's key.
 * @param   value           Pair's value.
 * @param   next            Reference to next pair.
 * @param   type            Pair's value's type.
 * @return  Allocated Pair structure.
 */

Pair *	    pair_create(const char *key, const Value value, Pair *next, Type type) {
    Pair *created = calloc(1, sizeof(Pair));
    if(created){
        created->key = strdup(key);
       
        if(type == STRING){
            created->value.string = strdup(value.string);
        }
        else{
            created->value.number = value.number;
        }
    
        created->type = type; 
        created->next = next;
    }
    
    
    return created;
}

/**
 * Delete Pair structure.
 * @param   p               Pair structure.
 * @param   recursive       Whether or not to delete remainder of pairs.
 * return   NULL.
 */
Pair *	    pair_delete(Pair *p, bool recursive) {
    if(p){
        if(recursive){
            if(p->next){
                pair_delete(p->next, recursive);
            }
        }
    
        free(p->key);
        if(p->type == STRING){
            free(p->value.string);
        }

        free(p);
    }

    return NULL;
}

/**
 * Update Pair's value.
 * @param   p               Pair structure.
 * @param   value           New value for Pair.
 * @param   type            New value's type.
 */
void        pair_update(Pair *p, const Value value, Type type) {
    if(p->type == STRING){
        free(p->value.string);
    }

    if(type == STRING){
        p->value.string = strdup(value.string);
    }
    else{
        p->value.number = value.number;
    }

    p->type = type;
    
}

/**
 * Format Pair by writing to stream.
 * @param   p               Pair structure.
 * @param   stream          File stream to write to.
 */
void        pair_format(Pair *p, FILE *stream) {
    char buffer[BUFSIZ];
    if(p->type == STRING){
        sprintf(buffer, "%s\t%s\n", p->key, p->value.string);
        fputs(buffer, stream);
    }
    else{
        sprintf(buffer, "%s\t%d\n", p->key, (int)p->value.number);
        fputs(buffer, stream);
    }
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
