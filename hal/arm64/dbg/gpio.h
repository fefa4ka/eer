#pragma once

#include <stdint.h>


typedef struct eer_pin {
    uint16_t number;
    char *        name;
    char          index;
    struct {
        uint16_t ddr;
        uint16_t port;
        uint16_t pin;
    } port;
} eer_pin_t;


void free_pins();
void dump_pin(eer_pin_t *pin);
void gpio_init();

extern struct hash_table pins;
