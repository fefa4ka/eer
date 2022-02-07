#pragma once

#include "avr.h"

typedef struct {
    struct {
        volatile unsigned char *ddr;
        volatile unsigned char *port;
        volatile unsigned char *pin;
    } port;
    unsigned char number;
} eer_pin_t;

#define eer_hw_pin(port, pin)                                                  \
    {                                                                          \
        {&DDR##port, &PORT##port, &PIN##port}, pin                             \
    }
