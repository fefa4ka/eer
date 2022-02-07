#include "lr.h"
#ifdef PROFILING
#include <profiler.h>
#endif

uint16_t lr_length_limited_owned(struct linked_ring *lr, uint16_t limit,
                                 lr_owner_t owner)
{
    uint8_t         length  = 0;
    struct lr_cell *counter = lr->read;
    struct lr_cell *needle  = lr->write ? lr->write : lr->read;

    /* Empty buffer */
    if (lr->read == 0)
        return 0;

    if (owner && (lr->owners & owner) != owner)
        return 0;

    /* Full buffer */
    if (!owner && lr->write == 0)
        return lr->size;


    /* Iterate from read to cell with *next = &lr.write */
    do {
        if (!owner || counter->owner == owner)
            length++;
        counter = counter->next;
    } while ((counter != needle) && (!limit || limit == length));

    return length;
}

eer_result_t lr_write(struct linked_ring *lr, lr_data_t data, lr_owner_t owner)
{
    struct lr_cell *recordable_cell;

#ifdef PROFILING
    eer_profiler_count_log(lr_write, "(%x, %x)", owner, data);
#endif

    if (!lr->write && lr->read)
        return ERROR_BUFFER_FULL;

    /* If free buffer */
    if (!lr->tail) {
        lr->tail       = lr->cells + lr->size;
        lr->tail->next = lr->cells;

        recordable_cell = lr->cells;
    } else {
        recordable_cell = lr->write;
    }

    recordable_cell->data  = data;
    recordable_cell->owner = owner;

    lr->owners |= owner;

    /* Initialize next pointer, if not set */
    if (!recordable_cell->next) {
        recordable_cell->next = recordable_cell + 1;
    }

    if (recordable_cell->next == lr->read)
        lr->write = 0;
    else
        lr->write = recordable_cell->next;

    if (!lr->read)
        lr->read = recordable_cell;

    return OK;
}


eer_result_t lr_read(struct linked_ring *lr, lr_data_t *data, lr_owner_t owner)
{
    struct lr_cell *readable_cell = lr->read;
    struct lr_cell *previous_cell = 0;
    struct lr_cell *freed_cell    = 0;
    struct lr_cell *needle        = lr->write ? lr->write : lr->read;

#ifdef PROFILING
    eer_profiler_count_log(lr_read, "(%x)", owner);
#endif

    if (lr_length_owned(lr, owner) == 0)
        return ERROR_BUFFER_EMPTY;

    /* Flush owners, and set again during buffer reading */
    /* O(n) = buffer_length */
    lr->owners = 0;

    do {
        struct lr_cell *next_cell = readable_cell->next;

        if (!owner || readable_cell->owner == owner) {
            *data = readable_cell->data;
            /* For skipping next match set impossible owner */
            // TODO: max lr_owner_t
            owner = 0xFFFF;

            /* Reassembly linking ring */
            if (previous_cell) {
                /* Link cells between */
                if (readable_cell->next == needle) {
                    lr->write           = readable_cell;
                    previous_cell->next = readable_cell;

                    return OK;
                } else {
                    previous_cell->next = readable_cell->next;
                }
            } else {
                /* If readed last available cell */
                if (readable_cell->next == needle) {
                    lr->read  = 0;
                    lr->write = readable_cell;

                    return OK;
                } else {
                    /* Once case when read pointer changing
                     * If readed first cell */
                    lr->read = readable_cell->next;
                    /* Link new read with last available write cell */
                    lr->tail->next = lr->read;
                }
            }

            freed_cell = readable_cell;
            if (lr->write)
                /* Add cell on top of the buffer */
                freed_cell->next = lr->write;
            else
                freed_cell->next = lr->read;

            if(needle == freed_cell)
                needle = lr->tail;
        } else {
            /* All cells owners digest */
            lr->owners |= readable_cell->owner;
        }

        /* Cell iteration */
        previous_cell = readable_cell;
        readable_cell = next_cell;
    } while (readable_cell != needle);

    if (owner != 0xFFFF) {
        return ERROR_BUFFER_EMPTY;
    } else {
        /* Last iteration. Link freed cell as next */
        previous_cell->next = freed_cell;

        if (!lr->write) {
            lr->tail = freed_cell;
        }
        lr->write = freed_cell;
    }

    return OK;
}

eer_result_t lr_write_string(struct linked_ring *lr, unsigned char *data,
                           lr_owner_t owner)
{
    while (*data) {
        if (lr_write(lr, *(data++), owner) == ERROR_BUFFER_FULL)
            return ERROR_BUFFER_FULL;
    };

    return OK;
}

#ifdef PROFILING
eer_result_t lr_dump(struct linked_ring *lr)
{
    if (lr_length(lr) == 0)
        return ERROR_BUFFER_EMPTY;

    printf("size=%d\n", lr_length(lr));
    struct lr_cell *readable_cell = lr->read;
    do {
        struct lr_cell *next_cell = readable_cell->next;

        printf("%x: %x | ", readable_cell->owner, readable_cell->data);
        readable_cell = next_cell;
    } while (readable_cell->next && readable_cell != lr->write);

    printf("\n");

    return OK;
}
#endif
