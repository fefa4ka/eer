#pragma once

#include <avr/interrupt.h>
#include <avr/io.h>

#include <hal.h>

typedef struct
{
    struct
    {
        volatile unsigned char *ddr;
        volatile unsigned char *port;
        volatile unsigned char *pin;
    } port;
    unsigned char number;
} eer_pin_t;

extern eer_hal hw;

#define hw_pin(port, pin)                                                     \
    {                                                                         \
        { &DDR##port, &PORT##port, &PIN##port }, pin                          \
    }

#define hw_isr_enable() sei()
#define hw_isr_disable() cli()

#define hw_uart_baudrate(baudrate)                                            \
    (((F_CPU) + 4UL * (baudrate)) / (8UL * (baudrate)) - 1UL)


#define log_pin(port, pin) \
    DDR##port |= (1 << pin); \
    PORT##port ^= (1 << pin); \
    PORT##port ^= (1 << pin);


#define debug(port, pin)                                                      \
    ({                                                                        \
        eer_pin_t debug_pin = hw_pin (port, pin);                                 \
        hw.io.out (&debug_pin);                                               \
        hw.io.flip (&debug_pin);                                              \
        hw.io.flip (&debug_pin);                                              \
    })
#define breakpoint(port, pin) while(true) log_pin(port, pin)
