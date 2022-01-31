#include "hash.h"

eer_result_t hash_read(struct hash_table *hash, unsigned int key, void **ptr)
{
    for (unsigned int index = 0; index <= hash->used; index++) {
        if (hash->index[index] == key) {
            *ptr = hash->data[index];

            return OK;
        }
    }
    return ERROR_BUFFER_EMPTY;
}

eer_result_t hash_pop(struct hash_table *hash, unsigned int key, void **ptr)
{
    for (unsigned int index = 0; index <= hash->used; index++) {
        if (hash->index[index] == key) {
            if (ptr)
                *ptr = (void *)hash->data[index];
            hash->data[index]  = hash->data[hash->used - 1];
            hash->index[index] = hash->index[hash->used - 1];
            hash->used -= 1;

            return OK;
        }
    }

    return ERROR_BUFFER_EMPTY;
}

eer_result_t hash_write(struct hash_table *hash, unsigned int key, void *data)
{
    for (unsigned int index = 0; index <= hash->used; index++) {
        if (hash->index[index] == key) {
            hash->data[index] = data;

            return OK;
        }
    }
    if (hash->size > hash->used) {
        hash->index[hash->used] = key;
        hash->data[hash->used]  = data;
        hash->used += 1;

        return OK;
    }

    return ERROR_BUFFER_FULL;
}
