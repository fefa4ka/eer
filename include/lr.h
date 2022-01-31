#pragma once

#include "macros.h"
#include "interface.h"

#define lr_data_t    uintptr_t
#define lr_data(ptr) (uintptr_t) ptr

#define lr_owner_t uintptr_t
/* Add for bit trimming */
#define lr_owner(ptr) ((uintptr_t)ptr + (uintptr_t)ptr)

struct lr_cell {
    lr_data_t       data;
    lr_owner_t      owner;
    struct lr_cell *next;
};

struct linked_ring {
    struct lr_cell *cells;
    unsigned int    size;

    lr_owner_t owners;

    struct lr_cell *read;
    struct lr_cell *write;
    struct lr_cell *tail;
};

uint16_t lr_length_limited_owned(struct linked_ring *, uint16_t limit,
                                 lr_owner_t owner);

#define lr_length(lr) lr_length_limited_owned(lr, 0, 0)
#define lr_exists(lr, owner) lr_length_limited_owned(lr, 1, owner)
#define lr_length_owned(lr, owner) lr_length_limited_owned(lr, 0, owner)

eer_result_t lr_write(struct linked_ring *lr, lr_data_t data, lr_owner_t owner);
eer_result_t lr_write_cells(struct linked_ring *lr, lr_data_t *data,
                           lr_owner_t owner);
eer_result_t lr_write_string(struct linked_ring *lr, unsigned char *data,
                           lr_owner_t owner);

eer_result_t lr_peek(struct linked_ring *, lr_data_t *, lr_owner_t);
eer_result_t lr_read(struct linked_ring *, lr_data_t *, lr_owner_t);
eer_result_t lr_read_filter(struct linked_ring *, lr_owner_t, lr_data_t *, void (*handler)(lr_data_t *, void *args), void *args);

eer_result_t lr_map(struct linked_ring *, lr_owner_t, void (*handler)(lr_data_t *, void *args), void *args);
eer_result_t lr_map_read(struct linked_ring *, lr_owner_t, void (*handler)(lr_data_t *, void *args), void *args);

#ifdef REACT_PROFILER
eer_result_t lr_dump(struct linked_ring *lr);
#endif
