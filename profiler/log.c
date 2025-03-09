#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <execinfo.h>

FILE *log_file;

void log_init() {
  if (!log_file)
    log_file = fopen("profiler.log", "w");

  if (log_file == NULL) {
    printf("Error: can't open profiler.log file: %d %s", errno,
           strerror(errno));
    exit(1);
  }
}

void log_printf(char *message, ...) {
  va_list args;
  va_start(args, message);
  vfprintf(log_file, message, args);
  va_end(args);
}

void log_clean() { fclose(log_file); }

// Returns the local date/time formatted as 2014-03-19 11:11:52.132
char *eer_timer_formatted_time(void) {
  // Must be static, otherwise won't work
  static char _retval[37];
  char _strtime[20];

  time_t rawtime;
  struct tm *timeinfo;
  struct timeval tv;

  int millisec;

  time(&rawtime);
  gettimeofday(&tv, NULL);
  timeinfo = localtime(&rawtime);

  millisec = lrint(tv.tv_usec / 1000.0);
  if (millisec >= 1000) {
    millisec -= 1000;
    tv.tv_sec++;
  }

  strftime(_strtime, sizeof(_strtime), "%Y-%m-%d %H:%M:%S", timeinfo);
  sprintf(_retval, "%s.%03d", _strtime, millisec);

  return _retval;
}
