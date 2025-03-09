#pragma once

#include "log.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define test_assert(test, message, ...)                                        \
  if (!(test)) {                                                               \
    log_error(message, ##__VA_ARGS__);                                         \
    return ERROR_UNKNOWN;                                                      \
  } else {                                                                     \
    log_ok(message, ##__VA_ARGS__);                                            \
  }

#define test_define_(test_method)                                              \
  eer_result_t test_method();                                                  \
  void *__##test_method(void *ptr) { return (void *)test_method(); }

#define test_define(...) EVAL(MAP(test_define_, __VA_ARGS__))

#define test_execute_(test_method)                                             \
  pthread_t test_method##_thread;                                              \
  int test_method##_thread_id;                                                 \
  pthread_create(&test_method##_thread, NULL, __##test_method,                 \
                 (void *)&test_method##_thread_id);

#define test_execute(...) EVAL(MAP(test_execute_, __VA_ARGS__))

#define test_wait_(test_method)                                                \
  pthread_join(test_method##_thread, (void **)&r);                             \
  if (r != 0) {                                                                \
    log_error(#test_method " test failed");                                    \
  }
#define test_wait(...) EVAL(MAP(test_wait_, __VA_ARGS__))

#undef eer_while
#define eer_while(...)                                                         \
  for (eer_land = (union eer_land){.state = {IF_ELSE(HAS_ARGS(__VA_ARGS__))(   \
                                       (EVAL(MAP(__eer_init, __VA_ARGS__))     \
                                            CONTEXT_SAME))(CONTEXT_UPDATED)}}; \
       !eer_land.state.unmounted && eer_land.state.context;                    \
       eer_land.state.context = IF_ELSE(HAS_ARGS(__VA_ARGS__))((               \
           EVAL(MAP(__eer_init, __VA_ARGS__)) CONTEXT_SAME))(CONTEXT_UPDATED))

#ifndef PROFILING
#undef eer_loop
#define eer_loop(...)                                                          \
  log_init();                                                                  \
  eer_boot:                                                                    \
  eer_while(__VA_ARGS__)
#endif

#define test_program(before, after, ...)                                       \
  union eer_land eer_land;                                                     \
  test_define(__VA_ARGS__);                                                    \
  void program();                                                              \
  void *__program(void *ptr) {                                                 \
    program();                                                                 \
    return NULL;                                                               \
  }                                                                            \
  int main(void) {                                                             \
    pthread_t program_thread;                                                  \
    int program_thread_id;                                                     \
    int r;                                                                     \
    before;                                                                    \
    pthread_create(&program_thread, NULL, __program,                           \
                   (void *)&program_thread_id);                                \
    test_execute(__VA_ARGS__);                                                 \
    test_wait(__VA_ARGS__);                                                    \
    eer_land.state.unmounted = true;                                           \
    pthread_join(program_thread, NULL);                                        \
    after;                                                                     \
    return r;                                                                  \
  }                                                                            \
  void program()

#define test_loop(number) for (int i = 0; i < number; i++)
#define test_loop_delay(number, order)                                         \
  for (int i = 0; i < number; i++, usleep(rand() % 10 * order))

#ifdef PROFILING
#define test(...) test_program({}, { eer_dump_usage(); }, __VA_ARGS__)
#else
#define test(...) test_program({}, { log_clean(); }, __VA_ARGS__)
#endif
