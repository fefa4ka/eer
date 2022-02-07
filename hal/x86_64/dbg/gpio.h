#pragma once

#include <stdint.h>


typedef struct eer_pin {
    uint8_t number;
    char *        name;
    char          index;
    struct {
        uint8_t ddr;
        uint8_t port;
        uint8_t pin;
    } port;
} eer_pin_t;


void free_pins();
void dump_pin(eer_pin_t *pin);
void gpio_init();

extern struct hash_table pins;
