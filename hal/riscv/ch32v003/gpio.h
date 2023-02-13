#pragma once

#include "ch32v003.h"

#define GPIOA_BASE (APB2PERIPH_BASE + 0x0800)
#define GPIOC_BASE (APB2PERIPH_BASE + 0x1000)
#define GPIOD_BASE (APB2PERIPH_BASE + 0x1400)

#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD ((GPIO_TypeDef *)GPIOD_BASE)

typedef struct {
    _rw uint32_t CFGLR;
    _rw uint32_t CFGHR;
    _rw uint32_t INDR;
    _rw uint32_t OUTDR;
    _rw uint32_t BSHR;
    _rw uint32_t BCR;
    _rw uint32_t LCKR;
} GPIO_TypeDef;

/* Output Maximum frequency selection */
typedef enum {
    GPIO_Speed_10MHz = 1,
    GPIO_Speed_2MHz,
    GPIO_Speed_50MHz
} GPIOSpeed_TypeDef;

typedef struct {
    GPIO_TypeDef *control;
    uint32_t      APBPeriph;
    uint8_t       number;
} eer_pin_t;

void gpio_init(void *pin, enum eer_pin_mode mode, GPIOSpeed_TypeDef speed);

#define eer_hw_pin(port, pin)                                                  \
    {                                                                          \
        GPIO##port, RCC_APB2Periph_GPIO##port, pin                             \
    }
