#include "profiler.h"
#include "test_utils.h"
#include <execinfo.h>
#include <math.h>

unsigned int scope_index[128];
eer_t *scope_buffer[128];

eer_t *current_component = 0;
struct eer_hal_calls *eer_current_scope = NULL;
struct eer_hal_calls eer_calls = {0};

struct hash_table eer_scope = {
    .index = scope_index,
    .data = (void **)&scope_buffer,
    .size = 128,
    .used = 0,
};

clock_t eer_cpu_total = 0;
void eer_signal_handler(int signal) { /* stop the event loop */ }

unsigned int eer_frame_depth() {
  const unsigned int max_depth = 200;
  void *buffer[max_depth];
  return backtrace(buffer, max_depth) - 3;
}

#define eer_dump_call(stage, call)                                             \
  if ((component)->calls.stage.call)                                           \
  printf("   " #call " \t \t%llu\n", component->calls.stage.call)

#define eer_dump_stage(stage)                                                  \
  if (component->counter.stage) {                                              \
    printf(" " #stage " \t \t%llu\t%llu\t%0.2lf%%\n",                          \
           component->counter.stage, component->cpu.stage,                     \
           ((double)component->cpu.stage / (double)eer_cpu_total) * 100);      \
    eer_dump_call(stage, some_call_counter);                                   \
  }

void eer_dump_usage() {
  eer_t *component;

  printf("\n\nCPU usage details:\n\n");
  printf("operation\t\tsteps\tcpu\t%%\n\n");
  printf("%d components\t\t%llu\t%lu\t100%%\n\n", eer_scope.used, eer_current_iteration,
         eer_cpu_total);
  for (unsigned int index = 0; index < eer_scope.used; index++) {
    component = (eer_t *)eer_scope.data[index];
    clock_t component_cpu_total =
        (component->cpu.will_mount + component->cpu.next_props +
         component->cpu.should_update + component->cpu.will_update +
         component->cpu.release + component->cpu.did_mount +
         component->cpu.did_update + component->cpu.did_unmount);
    double component_cpu_percent =
        ((double)component_cpu_total / (double)eer_cpu_total) * 100;
    printf("%d: ", index);
    printf("%s\n", component->name);
    eer_dump_stage(will_mount);
    eer_dump_stage(next_props);
    eer_dump_stage(should_update);
    eer_dump_stage(will_update);
    eer_dump_stage(release);
    eer_dump_stage(did_mount);
    eer_dump_stage(did_unmount);
    printf("\t\t\t\t%lu\t%0.2lf%%\n", component_cpu_total,
           component_cpu_percent);
    printf("\n\n");
  }

  log_clean();
}

const char *int_to_binary_str(int x, int N_bits) {
  static char b[512];
  char *p = b;
  b[0] = '\0';

  for (int i = (N_bits - 1); i >= 0; i--) {
    *p++ = (x & (1 << i)) ? '1' : '0';
    if (!(i % 4))
      *p++ = ' ';
  }
  return b;
}

unsigned int eer_hash_component(char *word) {
  unsigned long hash = 0;
  if (!word) {
    return 0;
  }
  while (*word != '\0') {
    hash += *word++;
  }

  return (hash % MAX_TABLE_SIZE);
}
