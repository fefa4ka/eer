#pragma once

#include <hal.h>
#include "log.h"
#include "vcd.h"
#include <version.h>
#ifdef PROFILING
    #include "profiler.h"
#endif

const char *int_to_binary_str(int x, int N_bits);

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
        hw(gpio).out(&debug_pin);                                              \
        hw(gpio).flip(&debug_pin);                                             \
        hw(gpio).flip(&debug_pin);                                             \
    })

#define eer_hw_pin(port, pin)                                                  \
    {                                                                          \
        pin, #port                                                             \
    }

#include "gpio.h"
extern eer_gpio_handler_t   eer_hw_gpio;
extern eer_timer_handler_t  eer_hw_timer;
extern eer_adc_handler_t    eer_hw_adc;
extern eer_serial_handler_t eer_hw_uart;
