#pragma once

#include "log.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


#define test_assert(test, message, ...)                                        \
    if (!(test)) {                                                             \
        log_error(message, ##__VA_ARGS__);                                     \
        return ERROR_UNKNOWN;                                                  \
    }

#define test_define_(test_method)                                              \
    eer_result_t test_method();                                                \
    void        *__##test_method(void *ptr) { return (void *)test_method(); }

#define test_define(...) EVAL(MAP(test_define_, __VA_ARGS__))

#define test_execute_(test_method)                                             \
    pthread_t test_method##_thread;                                            \
    int       test_method##_thread_id;                                         \
    pthread_create(&test_method##_thread, NULL, __##test_method,               \
                   (void *)&test_method##_thread_id);

#define test_execute(...) EVAL(MAP(test_execute_, __VA_ARGS__))

#define test_wait_(test_method)                                                \
    pthread_join(test_method##_thread, (void **)&r);                           \
    if (r != 0) {                                                            \
        log_error(#test_method " test failed");                                \
    }
#define test_wait(...) EVAL(MAP(test_wait_, __VA_ARGS__))

#ifdef PROFILING
    #define eer_stop_header
#else
    #define eer_stop_header bool eer_stop;
    #undef eer_loop
    #define eer_loop(...)                                                      \
        log_init();                                                            \
        vcd_init();                                                            \
        while (EVAL(MAP(__eer_loop, __VA_ARGS__)) !eer_stop)
#endif

// FIXME: avoid gpio_init and free_pins here
#define test_program(before, after, ...)                                       \
    eer_stop_header test_define(__VA_ARGS__);                                  \
    void            program();                                                 \
    void           *__program(void *ptr)                                       \
    {                                                                          \
        program();                                                             \
        return NULL;                                                           \
    }                                                                          \
    int main(void)                                                             \
    {                                                                          \
        pthread_t program_thread;                                              \
        int       program_thread_id;                                           \
        int       r;                                                           \
        before;                                                                \
        pthread_create(&program_thread, NULL, __program,                       \
                       (void *)&program_thread_id);                            \
        test_execute(__VA_ARGS__);                                             \
        test_wait(__VA_ARGS__);                                                \
        eer_stop = true;                                                       \
        pthread_join(program_thread, NULL);                                    \
        after;                                                                 \
        return r;                                                              \
    }                                                                          \
    void program()

#define test_loop(number) for (int i = 0; i < number; i++)
#define test_loop_delay(number, order)                                         \
    for (int i = 0; i < number; i++, usleep(rand() % 10 * order))
