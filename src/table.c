/* table.c: Separate Chaining Hash Table */

#include "table.h"
#include "hash.h"
#include "macros.h"

/**
 * Create Table data structure.
 * @param   capacity        Number of buckets in the hash table.
 * @return  Allocated Table structure.
 */
Table *	    table_create(size_t capacity) {
    Table *created = calloc(1, sizeof(Table));
    if(created){
        if(capacity == 0){
            created->capacity = DEFAULT_CAPACITY;
        }
        else{
            created->capacity = capacity;
        }
    
        created->buckets = calloc(created->capacity, sizeof(Pair));
        if(created->buckets)
            created->size = 0;
    }

    return created;
}

/**
 * Delete Table data structure.
 * @param   t               Table data structure.
 * @return  NULL.
 */
Table *	    table_delete(Table *t) {
    if(t){
        for(int i=0; i < t->capacity; i++){
            pair_delete(t->buckets[i].next, true);
        }
        free(t->buckets);
        free(t);
    }

    return NULL;
}

/**
 * Insert or update Pair into Table data structure.
 * @param   m               Table data structure.
 * @param   key             Pair's key.
 * @param   value           Pair's value.
 * @param   type            Pair's value's type.
 */
void        table_insert(Table *t, const char *key, const Value value, Type type) {
    if(t){
        int bucket = hash_from_data(key, strlen(key)) % t->capacity;
        bool match = false;
        for(Pair *curr = t->buckets[bucket].next; curr; curr= curr->next){
            if(streq(curr->key, key)){
                match = true;
                pair_update(curr, value, type);
                return;
            }

            if(curr->next == NULL && !match){
                curr->next = pair_create(key, value, NULL, type);
                t->size = t->size + 1;
                return;
            }
        }
        
        Pair *ptr = &t->buckets[bucket];
        ptr->next = pair_create(key, value, NULL, type);
        t->size = t->size + 1;

    }
}

Value *     table_search(Table *t, const char *key) {
    if(t){
        int bucket = hash_from_data(key, strlen(key)) % t->capacity;
        for(Pair *curr = t->buckets[bucket].next; curr; curr = curr->next){
            if(streq(curr->key, key)){
                return &(curr->value);
            }
        }
    }
    return NULL;
}

/**
 * Remove Pair from Table data structure with specified key.
 * @param   t               Table data structure.
 * @param   key             Key of the Pair to remove.
 * return   Whether or not the removal was successful.
 */
bool        table_remove(Table *t, const char *key) {
    if(t){
        if(table_search(t, key) != NULL){
            int bucket = hash_from_data(key, strlen(key)) % t->capacity;
            Pair *prev = &t->buckets[bucket];
            for(Pair *curr = t->buckets[bucket].next; curr; curr = curr->next){
                if(streq(curr->key, key)){
                    prev->next = curr->next;
                    pair_delete(curr, false);
                    t->size = t->size - 1;
                    return true;
                }
                prev = curr;
            }
        }
    }
                
    return false;
}

/**
 * Format all the entries in the Table data structure.
 * @param   m               Table data structure.
 * @param   stream          File stream to write to.
 */
void	    table_format(Table *t, FILE *stream) {
    if(t){
        for(int i = 0; i < t->capacity; i++){
            for(Pair *curr = t->buckets[i].next; curr; curr = curr->next){
               pair_format(curr, stream);
            }
        }
    } 
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
