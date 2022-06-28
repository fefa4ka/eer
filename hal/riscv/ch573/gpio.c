#include "gpio.h"

static void gpio_in(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;
    switch (Pin->mode) {
    case PIN_MODE_PULLUP:
        bit_clear(*Pin->port.drv, Pin->number);
        bit_set(*Pin->port.pu, Pin->number);
        break;

    case PIN_MODE_PULLDOWN:
        bit_set(*Pin->port.drv, Pin->number);
        bit_clear(*Pin->port.pu, Pin->number);
        break;

    default:
        bit_clear(*Pin->port.drv, Pin->number);
        bit_clear(*Pin->port.pu, Pin->number);
        break;
    }
    bit_clear(*Pin->port.dir, Pin->number);
}

static void gpio_out(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;
    switch (Pin->mode) {
    case PIN_MODE_PUSHPULL:
        bit_set(*Pin->port.drv, Pin->number);
        bit_set(*Pin->port.dir, Pin->number);
        break;

    default:
        bit_clear(*Pin->port.drv, Pin->number);
        bit_set(*Pin->port.dir, Pin->number);
        break;
    }
}

static void gpio_on(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_set(*Pin->port.out, Pin->number);
}

static void gpio_off(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_set(*Pin->port.clr, Pin->number);
}

static void gpio_flip(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_flip(*Pin->port.out, Pin->number);
}

static bool gpio_get(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    return *(Pin->port.pin) & (1 << (Pin->number));
}

eer_gpio_handler_t eer_hw_gpio = {
    .in   = gpio_in,
    .out  = gpio_out,
    .on   = gpio_on,
    .off  = gpio_off,
    .flip = gpio_flip,
    .get  = gpio_get,
    /* FIXME: implemet interrupts */
    //                 .isr = {
    //                    .enable = gpio_isr_enable,
    //                 },
};
