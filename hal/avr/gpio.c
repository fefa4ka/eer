#include "gpio.h"

static void gpio_in(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_clear(*Pin->port.ddr, Pin->number);
    bit_set(*Pin->port.port, Pin->number);
}

static void gpio_out(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_set(*Pin->port.ddr, Pin->number);
}

static void gpio_on(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_set(*Pin->port.port, Pin->number);
}

static void gpio_off(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_clear(*Pin->port.port, Pin->number);
}

static void gpio_flip(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_flip(*Pin->port.port, Pin->number);
}

static void gpio_pullup(void *pin) { gpio_on(pin); }

static bool gpio_get(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    return *(Pin->port.pin) & (1 << (Pin->number));
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
