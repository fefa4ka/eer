#include "vcd.h"
#include "gpio.h"
#include <hash.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define vcd_open(filename) fopen(#filename ".vcd", "w")


FILE *vcd_file;
FILE *vcd_file_log;


void vcd_init()
{
    if (vcd_file && vcd_file_log)
        return;

    // use appropriate location if you are using MacOS or Linux
    vcd_file     = vcd_open(profiler);
    vcd_file_log = vcd_open(profiler_log);

    if (vcd_file == NULL) {
        printf("Error: can't open vcd file");
        exit(1);
    }

    if (vcd_file_log == NULL) {
        printf("Error: can't open vcd file");
        exit(1);
    }
}

void vcd_clean()
{
    char c;
    fprintf(vcd_file, "$date"
                      "\n"
                      "   October 17, 2021."
                      "\n"
                      "$end"
                      "\n"
                      "$version"
                      "\n"
                      "   VCD generator tool version info text."
                      "\n"
                      "$end"
                      "\n"
                      "$comment"
                      "\n"
                      "   Any comment text."
                      "\n"
                      "$end"
                      "\n"
                      "$timescale 1us $end"
                      "\n"
                      "$scope module logic $end\n");
    for (unsigned int index = 0; index < pins.used; index++) {
        eer_pin_t *Pin = pins.data[index];
        fprintf(vcd_file, "$var wire 1 %c %s_%d$end\n", Pin->index, Pin->name,
                Pin->number);
    }

    fprintf(vcd_file, "$upscope $end"
                      "\n"
                      "$enddefinitions $end"
                      "\n");
    fclose(vcd_file_log);
    vcd_file_log = fopen("profiler_log.vcd", "r");
    if (vcd_file_log) {
        while ((c = fgetc(vcd_file_log)) != EOF)
            fputc(c, vcd_file);
        fclose(vcd_file_log);
        remove("profiler_log.vcd");
    } else {
        printf("Error on profiler_log.vcd generation: %d %s\n", errno,
               strerror(errno));
    }
    fclose(vcd_file);
}
