#pragma once

#include "interface.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct eer_sys_handler {
    void (*init)(void *config);
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

enum eer_sys_mode {
    SYS_MODE_BOOT,
    SYS_MODE_READY,
    SYS_MODE_SLEEP,
    SYS_MODE_WAKE,
    SYS_MODE_REBOOT,
    SYS_MODE_SHUTDOWN
};

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
    PIN_MODE_ANALOG,
    PIN_MODE_FLOAT,
    PIN_MODE_PULLUP,
    PIN_MODE_PULLDOWN,
    PIN_MODE_PUSHPULL,
    PIN_MODE_OPENDRAIN,
    PIN_FUNC_PUSHPULL,
    PIN_FUNC_OPENDRAIN
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


enum eer_dma_resource {
    DMA_MEMORY,
    DMA_PERIPHERAL
};

enum eer_dma_status {
    DMA_STATUS_ERROR,
    DMA_STATUS_IDLE,
    DMA_STATUS_BUSY,
    DMA_STATUS_HALFWAY,
    DMA_STATUS_COMPLETED,
    DMA_STATUS_COUNTER
};

enum eer_dma_size {
    DMA_SIZE_BYTE,
    DMA_SIZE_HALFWORD,
    DMA_SIZE_WORD
};

typedef struct eer_dma_isr {
    uint8_t             channel;
    enum eer_dma_status event : 4;
} eer_dma_isr_t;

typedef struct {
    uint8_t             channel;
    uint8_t mode;
    uint8_t length;
    struct {
        void *addr;
        enum eer_dma_resource type;
        enum eer_dma_size size;
    } source;
    struct {
        void *addr;
        enum eer_dma_resource type;
        enum eer_dma_size size;
    } dest;
} eer_dma_t;


typedef struct eer_dma_handler {
    void (*init)(void *config);
    void (*start)(void *channel);
    void (*stop)(void *channel);
    uint16_t (*status)(void *channel, int type);

    eer_isr_handler_t isr;
} eer_dma_handler_t;

typedef struct eer_serial_handler {
    void* (*init)(void *baudrate);
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
    void *(*get)(void *value); /* FIXME: Platform dependent? */
    void *(*delay_ns)(uint32_t ns_nr); /* FIXME: Platform dependent? */
    void *(*delay_us)(uint32_t us_nr); /* FIXME: Platform dependent? */
    void *(*delay_ms)(uint32_t us_nr); /* FIXME: Platform dependent? */
    uint16_t (*ticks_to_us)(uint16_t ticks);
    uint16_t (*us_to_ticks)(uint16_t us);

    eer_isr_handler_t isr;
} eer_timer_handler_t;


typedef struct eer_ble_handler {
    void(*init)(void *config);
} eer_ble_handler_t;




// Bit manipulation helpers
#define bit_pos(bit) (1 << bit)
#define bit_value(data, bit) ((data >> bit) & 1) // Return Data.Y value
#define bit_set(data, bit)   data |= (1 << bit)  // Set Data.Y to 1
#define bit_flip(data, bit)  data ^= (1 << bit)  // Set Data.Y to ! Data.Y
#define bit_clear(data, bit) data &= ~(1 << bit) // Clear Data.Y to 0
