/********************************** (C) COPYRIGHT
 ******************************** File Name          : core_riscv.h Author : WCH
 * Version            : V1.0.0
 * Date               : 2020/07/31
 * Description        : RISC-V Core Peripheral Access Layer Header File
 *******************************************************************************/
#pragma once
#include <hal.h>

#define __nop() asm volatile("nop")

typedef volatile uint8_t * uint8_ptr_t;
typedef volatile uint16_t *uint16_ptr_t;
typedef volatile uint32_t *uint32_ptr_t;

#define _r_              volatile const /* defines 'read only' permissions */
#define __w              volatile       /* defines 'write only' permissions */
#define _rw              volatile       /* defines 'read / write' permissions */
#define r__(index, size) uint8_t RESERVED##index[size]

/* PFIC (Program Fast Interrupt Controller)
 *
 * The current system manages 18 peripheral interrupt channels and 5 core
 * interrupt channels, and other interrupt sources are reserved.
 *
 * Unique fast interrupt response mechanism
 * - 4-way programmable direct interrupt vector address
 * - 18 peripheral interrupts, each interrupt request has independent trigger
 * and mask control bits, with dedicated status bits.
 * - 1 non-maskable interrupt NMI.
 *
 * Unique fast interrupt entry and exit mechanism,
 * hardware automatic stacking and recovery, no instruction overhead.
 */

/* memory mapped structures for PFIC */
struct __attribute__((packed)) PFIC {
    /* Interrupt Enable Status Register */
    _r_ uint32_t ISR[8];
    /* Interrupt Pending Status Register */
    _r_ uint32_t IPR[8];
    /* Interrupt Priority Threshold Configuration Register */
    _rw uint32_t ITHRESDR;
    /* Fast Interrupt Service Base Address Register */
    _rw uint32_t FIBADDRR;
    /* Interrupt Configuration Register */
    _rw uint32_t CFGR;
    /* Interrupt Global Status Register */
    _r_ uint32_t GISR;
    r__(0, 0x10);
    _rw uint32_t FIOFADDRR[4]; /* Fast Interrupt Offset Address Register */
    r__(1, 0x90);
    __w uint32_t IENR[8]; /* Interrupt Enable Setting Register */
    r__(2, 0x60);
    __w uint32_t IRER[8]; /* Interrupt Enable Clear Register */
    r__(3, 0x60);
    __w uint32_t IPSR[8]; /* Interrupt Pending Setup Register */
    r__(4, 0x60);
    __w uint32_t IPRR[8]; /* Interrupt Pending Clear Register */
    r__(5, 0x60);
    _rw uint32_t IACTR[8]; /* Interrupt Active Status Register */
    r__(6, 0xE0);
    _rw uint8_t IPRIOR[256]; /* Interrupt Priority Configuration Register */
    r__(7, 0x810);
    _rw uint32_t SCTLR; /* System Control Register */
};

typedef enum IRQn {
    Reset_IRQn    = 1,
    NMI_IRQn      = 2,  /* Non Maskable Interrupt   */
    EXC_IRQn      = 3,  /* Exceptions Interrupt     */
    SysTick_IRQn  = 12, /* System timer Interrupt  */
    SWI_IRQn      = 14, /* Software Interrupt */
    TMR0_IRQn     = 16,
    GPIO_A_IRQn   = 17,
    GPIO_B_IRQn   = 18,
    SPI0_IRQn     = 19,
    BLEB_IRQn     = 20,
    BLEL_IRQn     = 21,
    USB_IRQn      = 22,
    TMR1_IRQn     = 24,
    TMR2_IRQn     = 25,
    UART0_IRQn    = 26,
    UART1_IRQn    = 27,
    RTC_IRQn      = 28,
    ADC_IRQn      = 29,
    PWMX_IRQn     = 31,
    TMR3_IRQn     = 32,
    UART2_IRQn    = 33,
    UART3_IRQn    = 34,
    WDOG_BAT_IRQn = 35
} IRQn_Type;

/* Base address of PFIC related registers */
#define PFIC ((struct PFIC *)0xE000E000)


#ifdef HAL_gpio
    #include "gpio.h"
extern eer_gpio_handler_t eer_hw_gpio;
#endif
#ifdef HAL_timer
extern eer_timer_handler_t eer_hw_timer;
#endif
#ifdef HAL_uart
extern eer_serial_handler_t eer_hw_uart;
#endif


extern uint32_t irq_status;
void            eer_hw_isr_enable();
void            eer_hw_isr_disable();

static inline void eer_hw_isr_enable_system(IRQn_Type IRQn)
{
    PFIC->IENR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn)&0x1F));
}

static inline void eer_hw_isr_disable_system(IRQn_Type IRQn)
{
    uint32_t tem;
    tem                                 = PFIC->ITHRESDR;
    PFIC->ITHRESDR                      = 0x10;
    PFIC->IRER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn)&0x1F));
    PFIC->ITHRESDR                      = tem;
    __nop();
    __nop();
}
