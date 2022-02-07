#pragma once

#include <hal.h>
#include "log.h"
#include "vcd.h"

typedef struct eer_pin {
    unsigned char number;
    char         *name;
    char          index;
    struct {
        unsigned char ddr;
        unsigned char port;
        unsigned char pin;
    } port;
} eer_pin_t;

void free_pins();
void dump_pin(eer_pin_t *pin);
void gpio_init();

void log_init();
void log_clean();

void vcd_init();
void vcd_clean();

unsigned long time_in_ns();
unsigned long timer_get_ns();

const char *int_to_binary_str(int x, int N_bits);

void log_printf(char *message, ...);

#define test(...) test_program({ gpio_init(); }, { free_pins(); }, __VA_ARGS__)

#define hw_isr_enable()                                                        \
    {                                                                          \
    }
#define hw_isr_disable()                                                       \
    {                                                                          \
    }
#define debug(port, pin)                                                       \
    ({                                                                         \
        pin_t debug_pin = hw_pin(port, pin);                                   \
        hw(gpio).out(&debug_pin);                                                 \
        hw(gpio).flip(&debug_pin);                                                \
        hw(gpio).flip(&debug_pin);                                                \
    })

#define eer_hw_pin(port, pin)                                                      \
    {                                                                          \
        pin, #port                                                             \
    }

extern eer_gpio_handler_t eer_hw_gpio;
extern eer_timer_handler_t eer_hw_timer;
extern eer_adc_handler_t eer_hw_adc;
extern eer_serial_handler_t eer_hw_uart;
