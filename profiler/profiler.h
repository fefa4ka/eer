#pragma once

#include "hash.h"
#include "log.h"
#include "test_utils.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define MAX_TABLE_SIZE 10007 // Prime Number for hash table

struct eer_hal_calls {
  uint64_t some_call_counter;
  /* Add more calls here */
};

#define PROFILING_STRUCT                                                       \
  char name[128];                                                              \
  struct {                                                                     \
    uint64_t will_mount;                                                       \
    uint64_t next_props;                                                       \
    uint64_t should_update;                                                    \
    uint64_t will_update;                                                      \
    uint64_t release;                                                          \
    uint64_t did_mount;                                                        \
    uint64_t did_update;                                                       \
    uint64_t did_unmount;                                                      \
  } counter;                                                                   \
  struct {                                                                     \
    struct eer_hal_calls will_mount;                                           \
    struct eer_hal_calls next_props;                                           \
    struct eer_hal_calls should_update;                                        \
    struct eer_hal_calls will_update;                                          \
    struct eer_hal_calls release;                                              \
    struct eer_hal_calls did_mount;                                            \
    struct eer_hal_calls did_update;                                           \
    struct eer_hal_calls did_unmount;                                          \
  } calls;                                                                     \
  struct {                                                                     \
    uint64_t will_mount;                                                       \
    uint64_t next_props;                                                       \
    uint64_t should_update;                                                    \
    uint64_t will_update;                                                      \
    uint64_t release;                                                          \
    uint64_t did_mount;                                                        \
    uint64_t did_update;                                                       \
    uint64_t did_unmount;                                                      \
  } cpu;

#include "eer.h"

#define eer_profiler_count_log(func, format, ...)                              \
  log_verbose("(%p:%d) " #func "" format, __builtin_return_address(0),         \
              eer_frame_depth(), __VA_ARGS__);                                 \
  eer_profiler_counter(func)

#define eer_profiler_count(func)                                               \
  log_verbose("(%p:%d) " #func "()", __builtin_return_address(0),              \
              eer_frame_depth());                                              \
  eer_profiler_counter(func)

#define eer_profiler_counter(func)                                             \
  if (eer_current_scope) {                                                     \
    eer_current_scope->func += 1;                                              \
  } else {                                                                     \
    eer_calls.func += 1;                                                       \
  }

#define eer_profiler_tick(component, stage)                                    \
  clock_t begin, end, passed;                                                  \
  eer_current_scope = &(component)->instance.calls.stage;                      \
  log_verbose("%s." #stage, (component)->instance.name);                       \
  begin = clock()

#define eer_profiler_tock(component, stage)                                    \
  end = clock();                                                               \
  passed = end - begin;                                                        \
  eer_cpu_total += passed;                                                     \
  (component)->instance.cpu.stage += passed;                                   \
  (component)->instance.counter.stage++;                                       \
  eer_current_scope = NULL

#undef eer_define_component
#define eer_define_component(Type, instance_name)                              \
  {.stage = EER_STAGE_DEFINED,                                                 \
   .will_mount = Type##_will_mount,                                            \
   .should_update = Type##_should_update,                                      \
   .will_update = Type##_will_update,                                          \
   .release = Type##_release,                                                  \
   .did_mount = Type##_did_mount,                                              \
   .did_update = Type##_did_update,                                            \
   .did_unmount = Type##_did_unmount,                                          \
   .name = #instance_name " / " #Type}

#undef eer_lifecycle_prepare
#define eer_lifecycle_prepare(Type, instance, stage)                           \
  eer_profiler_tick((Type##_t *)instance, stage)

#undef eer_lifecycle_finish
#define eer_lifecycle_finish(Type, instance, stage)                            \
  eer_profiler_tock((Type##_t *)instance, stage);

#define profiler_init()                                                        \
  signal(SIGINT, eer_signal_handler);                                          \
  log_init();

#undef eer_init
#define eer_init(...)                                                          \
  profiler_init();                                                             \
  union eer_land eer_land = {0};                                               \
  ;                                                                            \
  eer_land.flags = 0;                                                          \
  eer_boot:                                                                    \
  eer_land.state.context = IF_ELSE(HAS_ARGS(__VA_ARGS__))((EVAL(               \
      MAP(__eer_init, __VA_ARGS__)) EER_CONTEXT_UPDATED))(EER_CONTEXT_UPDATED)

#undef eer_loop
#define eer_loop(...)                                                          \
  profiler_init();                                                             \
  eer_execute_hooks(EER_LOOP_BEFORE_START);                                    \
  eer_boot:                                                                    \
  eer_while(__VA_ARGS__)

#undef eer_terminate
#define eer_terminate                                                          \
  if (!eer_land.state.unmounted) {                                             \
    eer_increment_iteration();                                                 \
    goto eer_boot;                                                             \
  }

#undef eer_halt
#define eer_halt(code)                                                         \
  if (!eer_land.state.unmounted) {                                             \
    eer_increment_iteration();                                                 \
    goto eer_boot;                                                             \
  }                                                                            \
  {                                                                            \
    return code;                                                               \
  }

extern struct hash_table eer_scope;
extern struct eer_hal_calls *eer_current_scope;
extern clock_t eer_cpu_total;
extern struct eer_hal_calls eer_calls;
extern bool eer_stop;

void eer_signal_handler(int sig);
unsigned int eer_frame_depth();
void eer_dump_usage();
unsigned int eer_hash_component(char *word);
