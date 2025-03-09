#pragma once

#ifndef PROFILING
#define eer_steps (uint64_t)0 
#endif

#define __log_printf(type, message, ...)                                       \
  printf("%s\t" type "\t%lld\t" message "\n", eer_timer_formatted_time(),      \
         eer_steps, ##__VA_ARGS__)
#define log_print(type, message, ...)                                          \
  log_printf("%s\t" type "\t%lld\t" message "\n", eer_timer_formatted_time(),  \
             eer_steps, ##__VA_ARGS__)
#define log_debug(type, message, ...)                                          \
  log_print(type, message " (%s:%d)\n", ##__VA_ARGS__, __FILE__, __LINE__)
#define log_verbose(message, ...) log_print("VERBOSE", message, ##__VA_ARGS__)
#define log_info(message, ...) log_print("INFO", message, ##__VA_ARGS__)
#define log_ok(message, ...) log_print("OK", message, ##__VA_ARGS__)
#define log_error(message, ...)                                                \
  log_debug("ERROR", message, ##__VA_ARGS__);                                  \
  __log_printf("\e[1m\e[31mERROR\e[39m\e[0m", message " (%s:%d)\n",            \
               ##__VA_ARGS__, __FILE__, __LINE__)

char *get_formatted_time(void);
void log_init();
void log_clean();
void log_printf(char *message, ...);
char *eer_timer_formatted_time(void);
