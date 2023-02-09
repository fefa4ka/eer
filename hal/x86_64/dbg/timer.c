#include "dbg.h"
#include "timer.h"
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>

/* Time */
static void timer_init(void *config)
{
    eer_profiler_count(timer_init);
    srand(time(NULL));
}

long          tick = 0;
unsigned long time_in_ns()
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1e9 + now.tv_nsec;
}

unsigned long timer_get_ns()
{
    long tock   = time_in_ns();
    long passed = tock - tick;

    if (tick) {
        tick = tock;
        return passed;
    }

    tick = tock;
    return 0;
}

static inline void *timer_get(void *value)
{
    clock_t tick = clock();
    // printf("%ld - %ld\n", tick, timer_get_ns());
    eer_profiler_count_log(timer_get, "()=%d", tick);
   // printf("Timer get: %ld, %ld, %d\r\n", tick, CLOCKS_PER_SEC, tick * 1000 /
   // CLOCKS_PER_SEC);
    *(uint64_t*)value = tick;

    return value;
}

struct timer_callback {
    unsigned int timeout;
    eer_result_t (*callback)(void *argument, void *trigger);
    void *    args;
    size_t    index;
    pthread_t thread;
};

#define TIMERS_NR 10
struct timer_callback timer_callback_buffer[TIMERS_NR] = {0};

void *timer_timeout(void *ptr)
{
    struct timer_callback *callback = ptr;

    usleep(callback->timeout);
    printf("sleep %d\n", callback->timeout);
    callback->callback(callback->args, 0);
    *callback = (struct timer_callback){0};

    return NULL;
}

static void timer_compare_set(uint16_t ticks, eer_callback_t *callback)
{
    uint16_t now = timer_get();
    eer_profiler_count(timer_compare_set);
    for (size_t i = 0; i < TIMERS_NR; i++) {
        if (timer_callback_buffer[i].callback == NULL) {
            struct timer_callback timer_callback
                = {ticks - now, callback->method, callback->argument, i};
            struct timer_callback *timer = timer_callback_buffer + i;
            *timer                       = timer_callback;
            pthread_create(&timer->thread, NULL, *timer_timeout, (void *)timer);

            // printf("Timer #%ld set: %d\r\n", i, ticks);
            break;
        }
    }
}

void *timer_isr_enable(void *args_ptr, eer_callback_t *callback)
{
    struct eer_timer_isr *args = (struct eer_timer_isr *)args_ptr;
    if (args->event == TIMER_EVENT_COMPARE) {
        timer_compare_set(args->ticks, callback);

        return timer_compare_set;
    }

    return 0;
}


static void timer_off()
{
    eer_profiler_count(timer_off);
    // printf("Timer off\r\n");
}

static uint16_t timer_ticks_to_us(uint16_t ticks)
{
    uint16_t ms = ticks * 1000 / CLOCKS_PER_SEC;
    eer_profiler_count_log(timer_ticks_to_us, "(%d)=%d ms", ticks, ms);
    return ticks;
}

static uint16_t timer_us_to_ticks(uint16_t us)
{
    uint16_t ms = us * CLOCKS_PER_SEC / 1000;
    eer_profiler_count_log(timer_us_to_ticks, "(%d)=%d", us, us);
    return us;
}


eer_timer_handler_t eer_hw_timer = {.init        = timer_init,
                                    .get         = timer_get,
                                    .ticks_to_us = timer_ticks_to_us,
                                    .us_to_ticks = timer_us_to_ticks,
                                    .isr         = {
                                        .enable  = timer_isr_enable,
                                        .disable = timer_off,
                                    }};
