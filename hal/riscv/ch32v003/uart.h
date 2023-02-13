#pragma once

#include "sys.h"
#include "gpio.h"

/* Universal Synchronous Asynchronous Receiver Transmitter */
typedef struct {
    _rw uint16_t STATR;
    uint16_t     RESERVED0;
    _rw uint16_t DATAR;
    uint16_t     RESERVED1;
    _rw uint16_t BRR;
    uint16_t     RESERVED2;
    _rw uint16_t CTLR1;
    uint16_t     RESERVED3;
    _rw uint16_t CTLR2;
    uint16_t     RESERVED4;
    _rw uint16_t CTLR3;
    uint16_t     RESERVED5;
    _rw uint16_t GPR;
    uint16_t     RESERVED6;
} USART_TypeDef;
