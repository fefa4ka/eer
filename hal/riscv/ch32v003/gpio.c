#include "gpio.h"
#include <stdio.h>

/* Configuration Mode enumeration */
typedef enum {
    GPIO_Mode_AIN         = 0x0,
    GPIO_Mode_IN_FLOATING = 0x04,
    GPIO_Mode_IPD         = 0x28,
    GPIO_Mode_IPU         = 0x48,
    GPIO_Mode_Out_OD      = 0x14,
    GPIO_Mode_Out_PP      = 0x10,
    GPIO_Mode_AF_OD       = 0x1C,
    GPIO_Mode_AF_PP       = 0x18
} GPIOMode_TypeDef;

#define is_bit_low(bit)   ((uint32_t)bit_pos(bit) & ((uint32_t)0x00FF)) != 0x00
#define is_bit_high(bit)  (uint32_t) bit_pos(bit) > 0x00FF
#define bit_pos_high(bit) (((uint32_t)1) << (bit + 0x08));


/* Chapter 7 GPIO and Alternate function (GPIO/AFIO)
 * Each pin of the port can be configured to one of the following multiple modes.
 *
 * Floating input
 * Pull-up input
 * Dropdown input
 * Analog input
 * Open drain output
 * Push-pull output
 * Multiplexing the inputs and outputs of functions
*/
void gpio_init(void *pin, enum eer_pin_mode mode, GPIOSpeed_TypeDef speed)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    uint32_ptr_t port_cfg_ptr;
    uint32_t     port_cfg;
    uint32_t     config;

    uint32_t pin_mask, pin_mode_pos, pin_cfg_pos;

    RCC->APB2PCENR |= Pin->APBPeriph;

    if (is_bit_low(Pin->number)) {
        port_cfg_ptr = &(Pin->control->CFGLR);
        pin_mode_pos = bit_pos(Pin->number);
    } else if (is_bit_high(Pin->number)) {
        port_cfg_ptr = &(Pin->control->CFGHR);
        pin_mode_pos = bit_pos_high(Pin->number);
    }

    port_cfg = *port_cfg_ptr;

    pin_cfg_pos = Pin->number << 2;
    pin_mask    = ((uint32_t)0x0F) << pin_cfg_pos;
    port_cfg &= ~pin_mask;

    switch (mode) {
    case PIN_MODE_FLOAT:
        config = ((uint32_t)GPIO_Mode_IN_FLOATING);
        break;

    case PIN_MODE_PULLUP:
        config             = ((uint32_t)GPIO_Mode_IPU);
        Pin->control->BSHR = pin_mode_pos;
        break;

    case PIN_MODE_PULLDOWN:
        config            = ((uint32_t)GPIO_Mode_IPD);
        Pin->control->BCR = pin_mode_pos;
        break;

    case PIN_MODE_PUSHPULL:
        config = ((uint32_t)GPIO_Mode_Out_PP);
        break;

    case PIN_MODE_OPENDRAIN:
        config = ((uint32_t)GPIO_Mode_Out_OD);
        break;

    case PIN_FUNC_PUSHPULL:
        config = ((uint32_t)GPIO_Mode_AF_PP);
        break;

    case PIN_FUNC_OPENDRAIN:
        config = ((uint32_t)GPIO_Mode_AF_OD);
        break;

    default:
        break;
    }

    if((((uint32_t)config) & ((uint32_t)0x10))) {
        /* If output */
        config &= (uint32_t)0x0F;
        config |= speed;
    } else {
        config &= (uint32_t)0x0F;
    }

    port_cfg |= (config << pin_cfg_pos);
    *port_cfg_ptr = port_cfg;
}

static void gpio_in(void *pin) { gpio_init(pin, PIN_MODE_FLOAT, 0); }

static void gpio_pullup(void *pin) { gpio_init(pin, PIN_MODE_PULLUP, 0); }

static void gpio_pulldown(void *pin) { gpio_init(pin, PIN_MODE_PULLDOWN, 0); }

static void gpio_out(void *pin) { gpio_init(pin, PIN_MODE_PUSHPULL, GPIO_Speed_50MHz); }

static void gpio_on(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    Pin->control->BSHR = bit_pos(Pin->number);
}

static void gpio_off(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    Pin->control->BCR = bit_pos(Pin->number);
}

static void gpio_flip(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    if ((Pin->control->OUTDR & bit_pos(Pin->number)) >> Pin->number) {
        Pin->control->BCR = bit_pos(Pin->number);
    } else {
        Pin->control->BSHR = bit_pos(Pin->number);
    }
}

static bool gpio_get(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    return (bool)((Pin->control->INDR & bit_pos(Pin->number)) >> Pin->number);
    /* TODO: if output */
    // return (bool)((*(Pin->control->OUTDR) & bit_pos(Pin->number)) >>
    // (Pin->number));
}

eer_gpio_handler_t eer_hw_gpio = {.in       = gpio_in,
                                  .out      = gpio_out,
                                  .on       = gpio_on,
                                  .off      = gpio_off,
                                  .flip     = gpio_flip,
                                  .get      = gpio_get,
                                  .pullup   = gpio_pullup,
                                  .pulldown = gpio_pulldown};
