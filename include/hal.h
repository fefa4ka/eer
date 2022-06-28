#pragma once

#include "interface.h"
#include <stdbool.h>
#include <stdint.h>


typedef struct eer_sys_handler {
    struct {
        void (*set)(void *unit);
        uint32_t (*get)(void *unit);
    } clock;
} eer_sys_handler_t;

typedef struct eer_isr_handler {
    bool (*is_available)(void *trigger);
    void *(*enable)(void *isr_args, eer_callback_t *);
    void (*disable)(void *isr_args);
} eer_isr_handler_t;

typedef struct eer_gpio_handler {
    void (*in)(void *pin);
    void (*out)(void *pin);
    void (*on)(void *pin);
    void (*off)(void *pin);
    void (*flip)(void *pin);
    void (*pullup)(void *pin);
    void (*pulldown)(void *pin);
    bool (*get)(void *pin);

    eer_isr_handler_t isr;
} eer_gpio_handler_t;

enum eer_communication_mode {
    COMMUNICATION_MODE_DISABLED,
    COMMUNICATION_MODE_TRANSMITTER,
    COMMUNICATION_MODE_RECEIVER
};

enum eer_bit_order { BIT_ORDER_LSB, BIT_ORDER_MSB };

enum eer_io_event {
    IO_EVENT_RISE,
    IO_EVENT_FALL,
    IO_EVENT_TOGGLE,
};

enum eer_io_level { IO_LEVEL_LOW, IO_LEVEL_HIGH };

enum eer_pin_mode {
    PIN_MODE_OFF,
    PIN_MODE_OUTPUT,
    PIN_MODE_INPUT,
    PIN_MODE_PULLUP,
    PIN_MODE_PULLDOWN,
    PIN_MODE_PUSHPULL
};

struct eer_io_isr {
    void *            pin;
    enum eer_io_event event : 3;
};

typedef struct eer_adc_handler {
    void (*mount)(void *prescaler);
    void (*select_channel)(void *channel);
    void (*start_convertion)(void *channel);
    bool (*is_convertion_ready)(void *channel);
    int16_t (*read_convertion)(void *channel);

    eer_isr_handler_t isr;
} eer_adc_handler_t;

typedef struct eer_serial_handler {
    void (*init)(void *baudrate);
    bool (*is_data_received)();
    bool (*is_transmit_ready)();
    void (*transmit)(unsigned char data);
    unsigned char (*receive)();

    eer_isr_handler_t isr;
} eer_serial_handler_t;

enum eer_timer_event {
    TIMER_EVENT_COMPARE,
    TIMER_EVENT_UPDATE,
    TIMER_EVENT_CAPTURE
};

typedef struct eer_timer_isr {
    uint16_t             ticks;
    enum eer_timer_event event : 3;
} eer_timer_isr_t;

typedef struct eer_timer_handler {
    void (*init)(void *config);
    uint16_t (*get)();
    uint16_t (*ticks_to_us)(uint16_t ticks);

    eer_isr_handler_t isr;
} eer_timer_handler_t;

// Bit manipulation helpers
#define bit_value(data, bit) ((data >> bit) & 1) // Return Data.Y value
#define bit_set(data, bit)   data |= (1 << bit)  // Set Data.Y to 1
#define bit_flip(data, bit)  data ^= (1 << bit)  // Set Data.Y to ! Data.Y
#define bit_clear(data, bit) data &= ~(1 << bit) // Clear Data.Y to 0
