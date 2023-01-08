#include "dbg.h"

static void sys_set_clock(void *clock)
{
}

static uint32_t sys_get_clock(void *unit)
{
    return 0;
}

eer_sys_handler_t eer_hw_sys = {
    .clock = {
        .set = sys_set_clock,
        .get = sys_get_clock,
    },
};
