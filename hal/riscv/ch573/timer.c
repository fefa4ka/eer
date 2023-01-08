#include "ch573.h"

/*
 * The kernel comes with a 64-bit down-counter (SysTick), which supports HCLK or
 * HCLK/8 as the time base, has a higher priority, and can be used for time
 * reference after calibration.
 */

/* memory mapped structure for SysTick */
struct __attribute__((packed)) SysTick {
    /* System Counter Control Register */
    union {
        struct {
            /* Bit 0: System counter enable control bit */
            _rw uint32_t STE : 1; /* 1: Enable system counter STK;
                                   * 0: Disable system counter STK, the counter
                                   * stops counting. */

            /* Bit 1: Counter interrupt enable control bit */
            _rw uint32_t STIE : 1; /* 1: Enable counter interrupt;
                                    * 0: No counter interrupt */

            /* Bit 2: Counter clock source selection */
            _rw uint32_t STCLK : 1; /* 1: HCLK as time base;
                                     * 0: HCLK/8 as counting time base */
            uint32_t reversed1 : 5;

            /* Bit 8: Reload control
             * Write 1 to update the count reload register (64 bit), value is
             * updated to the current counter register. */
            __w uint32_t STRELOAD : 1;

            uint32_t reversed2 : 23;
        } b;
        _rw uint32_t w;
    } CTLR;

    /* System Counter Register */
    _rw uint64_t CNT; /* Count down.
                       * The STK interrupt flag is set when the CNT[63:0] value
                       * decreases to 0. */

    /* Count Reload Register */
    _rw uint64_t CMP;

    /* Counter Counting Flag Register */
    struct {
        r__(1, 0x1E);
        /* The counter is decremented to 0 flag. Write 0 to clear, write 1 to
         * have no effect. */
        _rw uint32_t CNTIF : 1;

        /* System software interrupt enable bit. */
        _rw uint32_t SWIE : 1; /* 1: Enable software IRQ and trigger software
                                * IRQ service;
                                * 0: Disable software interrupt. */
    } CNTFG;
};

/* STK related register base address */
#define SysTick ((struct SysTick *)0xE000F000)

#define SysTick_LOAD_RELOAD_Msk    (0xFFFFFFFFFFFFFFFF)
#define SysTick_CTRL_RELOAD_Msk    (1 << 8)
#define SysTick_CTRL_CLKSOURCE_Msk (1 << 2)
#define SysTick_CTRL_TICKINT_Msk   (1 << 1)
#define SysTick_CTRL_ENABLE_Msk    (1 << 0)

static void timer_init(void *config) {
    /* Enable system counter STK */
    SysTick->CTLR.w
        = SysTick_CTRL_RELOAD_Msk | SysTick_CTRL_CLKSOURCE_Msk
          | SysTick_CTRL_TICKINT_Msk
          | SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */
}

static uint64_t timer_get() { return SysTick->CNT; }

static void (*_timer_compare_func)(void *trigger, void *argument);
static void *_timer_compare_func_arg = 0;

__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode"))) void
SysTick_Handler(void)
{
    _timer_compare_func(_timer_compare_func_arg, SysTick_Handler);
}

static void timer_compare_set(uint64_t ticks, eer_callback_t *callback)
{
    if ((ticks - 1) > SysTick_LOAD_RELOAD_Msk)
        return; /* Reload value impossible */

    _timer_compare_func     = callback->method;
    _timer_compare_func_arg = callback->argument;

    SysTick->CMP = ticks - 1; /* set reload register */
    PFIC->IENR[((uint32_t)(SysTick_IRQn) >> 5)]
        = (1 << ((uint32_t)(SysTick_IRQn) & 0x1F));

    SysTick->CTLR.w
        = SysTick_CTRL_RELOAD_Msk | SysTick_CTRL_CLKSOURCE_Msk
          | SysTick_CTRL_TICKINT_Msk
          | SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */

    /*
     * TODO: Test optimization
    SysTick->CTLR.b.STE =
    SysTick->CTLR.b.STIE =
    SysTick->CTLR.b.STCLK =
    SysTick->CTLR.b.STRELOAD = true;
    */
}

static void *timer_isr_enable(void *args_ptr, eer_callback_t *callback)
{
    struct eer_timer_isr *args = (struct eer_timer_isr *)args_ptr;
    if (args->event == TIMER_EVENT_COMPARE) {
        timer_compare_set(args->ticks, callback);

        return SysTick_Handler;
    }
}

static void timer_off()
{
    uint32_t tem;
    tem            = PFIC->ITHRESDR;
    PFIC->ITHRESDR = 0x10;
    PFIC->IRER[((uint32_t)(SysTick_IRQn) >> 5)]
        = (1 << ((uint32_t)(SysTick_IRQn)&0x1F));
    PFIC->ITHRESDR = tem;
    __nop();
    __nop();
}

static uint16_t timer_ticks_to_us(uint16_t ticks) { return ticks >> 9; }

eer_timer_handler_t eer_hw_timer = {.init        = timer_init,
                                    .get         = timer_get,
                                    .ticks_to_us = timer_ticks_to_us,
                                    .isr         = {
                                        .enable  = timer_isr_enable,
                                        .disable = timer_off,
                                    }};
