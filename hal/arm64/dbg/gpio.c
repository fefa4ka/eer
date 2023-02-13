#include "gpio.h"
#include <hal.h>
#include "log.h"
#include "vcd.h"
#include <errno.h>
#include <hash.h>
#include <math.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#ifdef PROFILING
    #include "profiler.h"
#endif


#define MAX_TABLE_SIZE 10007 // Prime Number
pthread_mutex_t get_pin_lock;

static char  pin_index = 33;
unsigned int pins_index[128];
eer_pin_t *  pins_buffer[128];

struct hash_table pins = {
    .index = pins_index,
    .data  = (void **)&pins_buffer,
    .size  = 128,
    .used  = 0,
};


unsigned int hash_pin(eer_pin_t *pin)
{
    unsigned long hash = 0;
    char *        word = pin->name;

    while (*word != '\0') {
        hash = *word++ + 31 * hash;
    }
    hash += pin->number;

    // log_info("%s_%d=%d", pin->name, pin->number, hash % MAX_TABLE_SIZE);
    return (hash % MAX_TABLE_SIZE);
}

void gpio_thread() {}
void gpio_init()
{
    log_init();
    vcd_init();
    pthread_mutex_init(&get_pin_lock, NULL);
    // TODO: thread with gpio manipulation
}

static eer_pin_t *get_pin(eer_pin_t *pin)
{
    eer_pin_t *Pin;
    pthread_mutex_lock(&get_pin_lock);
    if (hash_read(&pins, hash_pin(pin), (void **)&Pin) == OK) {
        pthread_mutex_unlock(&get_pin_lock);
        // printf("Pin PORT_%s_%d — cached\r\n", Pin->name, Pin->number);
        return Pin;
    }

    Pin            = malloc(sizeof(eer_pin_t));
    Pin->name      = strdup(pin->name);
    Pin->number    = pin->number;
    Pin->port.port = 0;
    Pin->port.pin  = 0;
    Pin->port.ddr  = 0;

    // printf("%s - %d - %d\n", pin->name, hash_pin(pin), hash_pin(Pin));
    hash_write(&pins, hash_pin(Pin), Pin);

    Pin->index = pin_index++;
    // printf("Pin PORT_%s_%d = (#%d %x) new\r\n", Pin->name, Pin->number,
    // hash_pin(pin), &(Pin->port));
    pthread_mutex_unlock(&get_pin_lock);
    return Pin;
}

static bool gpio_get(void *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);

    eer_profiler_count_log(gpio_get, "(%s_%d)=%d", Pin->name, Pin->number,
                           (Pin->port.pin) & (1 << Pin->number));

    return (Pin->port.pin) & (1 << Pin->number);
}

void dump_pin(eer_pin_t *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);
    printf("Pin PORT_%s_%d #%d = %d\n", Pin->name, Pin->number, hash_pin(pin),
           gpio_get(pin));
}

void free_pins()
{
    for (unsigned int index = 0; index < pins.used; index++) {
        eer_pin_t *Pin = pins.data[index];
        free(Pin->name);
        free(Pin);
    }
    pthread_mutex_destroy(&get_pin_lock);
#ifndef PROFILING
    vcd_clean();
    log_clean();
#endif
    system("/bin/stty cooked");
}

static void gpio_in(void *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);

    bit_clear(Pin->port.ddr, Pin->number);
    bit_set(Pin->port.port, Pin->number);

    eer_profiler_count_log(gpio_in, "(%s_%d)", Pin->name, Pin->number);
    // printf("Pin PORT_%s_%d — in\r\n", Pin->port, Pin->number);
}

static void gpio_out(void *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);

    bit_set(Pin->port.ddr, Pin->number);

    eer_profiler_count_log(gpio_out, "(%s_%d)", Pin->name, Pin->number);
    // printf("Pin PORT_%s_%d — out\r\n", Pin->port, Pin->number);
}

static void gpio_on(void *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);

    clock_t current_time = clock();
    if (!((Pin->port.pin) & (1 << Pin->number))) {
        fprintf(vcd_file_log, "#%ld\n%d%c\n", current_time, 1, Pin->index);
    }
    bit_set(Pin->port.pin, Pin->number);

    // printf("On %x ", &(Pin->port));
    // dump_pin(pin);
    // printf("Pin PORT_%s_%d — on - %d - %d\r\n", Pin->name, Pin->number,
    // Pin->port.port, 1 << Pin->number);
    eer_profiler_count_log(gpio_on, "(%s_%d)", Pin->name, Pin->number);
}

static void gpio_off(void *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);

    clock_t current_time = clock();
    if (((Pin->port.pin) & (1 << Pin->number))) {
        fprintf(vcd_file_log, "#%ld\n%d%c\n", current_time, 0, Pin->index);
    }
    bit_clear(Pin->port.pin, Pin->number);

    // printf("Off %x ", &(Pin->port));
    // dump_pin(pin);
    // printf("Pin PORT_%s_%d — off - %d\r\n", Pin->name, Pin->number,
    // Pin->port.port);
    eer_profiler_count_log(gpio_off, "(%s_%d)", Pin->name, Pin->number);
}

static void gpio_flip(void *pin)
{
    eer_pin_t *Pin          = get_pin((eer_pin_t *)pin);
    clock_t    current_time = clock();

    bit_flip(Pin->port.pin, Pin->number);
    fprintf(vcd_file_log, "#%ld\n%d%c\n", current_time, gpio_get(pin),
            Pin->index);
    eer_profiler_count_log(gpio_flip, "(%s_%d)", Pin->name, Pin->number);
}

static void gpio_pullup(void *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);

    bit_set(Pin->port.pin, Pin->number);
    eer_profiler_count_log(gpio_pullup, "(%s_%d)", Pin->name, Pin->number);
}

eer_gpio_handler_t eer_hw_gpio = {
    .in     = gpio_in,
    .out    = gpio_out,
    .on     = gpio_on,
    .off    = gpio_off,
    .flip   = gpio_flip,
    .get    = gpio_get,
    .pullup = gpio_pullup,
    //                 .isr = {
    //                    .enable = gpio_isr_enable,
    //                 },
};
