#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>


FILE *log_file;


void log_init()
{
    if (!log_file)
        log_file = fopen("profiler.log", "w");

    if (log_file == NULL) {
        printf("Error: can't open profiler.log file: %d %s", errno,
               strerror(errno));
        exit(1);
    }
}

void log_printf(char *message, ...)
{
    va_list args;
    va_start(args, message);
    vfprintf(log_file, message, args);
    va_end(args);
}

void log_clean() { fclose(log_file); }

