#pragma once

#include "interface.h"

struct hash_table {
    unsigned int *index;
    void        **data;
    unsigned int  size;
    unsigned int  used;
};

eer_result_t hash_read(struct hash_table *, unsigned int key, void **ptr);
eer_result_t hash_write(struct hash_table *, unsigned int key, void *data);
eer_result_t hash_pop(struct hash_table *, unsigned int key, void **ptr);
