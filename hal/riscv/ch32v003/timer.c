#include "ch32v003.h"

/* memory mapped structure for SysTick */
typedef struct {
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
            /* Bit 3: Reload control
             * Write 1 to update the count reload register (64 bit), value is
             * updated to the current counter register. */
            __w uint32_t STRELOAD : 1;

            uint32_t reversed1 : 27;
            /* Software interrupt trigger enable (SWI).
             * 1: Triggering software interrupts.
             * 0: Turn off the trigger.
             * After entering software interrupt, software clear 0 is required,
             * otherwise it is continuously triggered.
             */
            __w uint32_t SWIE : 1;
        } b;
        _rw uint32_t w;
    } CTLR;
    /* 6.5.4.2 System count status register */
    /* Count value comparison flag, write 0 to clear, write 1 to invalidate.
     *
     * 1: Up count reaches the comparison value.
     * 0: The comparison value is not reached.
     */
    _rw uint32_t SR;
    /* The current counter count value is 32 bits. */
    _rw uint32_t CNT;
    uint32_t     RESERVED0;
    /* Set the comparison counter value to 32 bits. */
    _rw uint32_t CMP;
    uint32_t     RESERVED1;
} SysTick_Type;

/* STK related register base address */
#define SysTick         ((SysTick_Type *) 0xE000F000)

#define SysTick_LOAD_RELOAD_Msk    (0xFFFFFFFFFFFFFFFF)
#define SysTick_CTRL_RELOAD_Msk    (1 << 3)
#define SysTick_CTRL_CLKSOURCE_Msk (1 << 2)
#define SysTick_CTRL_TICKINT_Msk   (1 << 1)
#define SysTick_CTRL_ENABLE_Msk    (1 << 0)

static uint8_t  p_us = 0;
static uint16_t p_ms = 0;

static void timer_init(void *config)
{
    eer_hw_isr_enable_system(SysTick_IRQn);
    /* Enable system counter STK */
    SysTick->CTLR.w = SysTick_CTRL_RELOAD_Msk | SysTick_CTRL_CLKSOURCE_Msk
          | SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */

    p_us = SystemCoreClock / 1e6;
    p_ms = (uint16_t)p_us * 1000;
}

static void *timer_get(void *value)
{
    *(uint64_t *)value = SysTick->CNT;

    return value;
}


static void (*_timer_compare_func)(void *trigger, void *argument);
static void *_timer_compare_func_arg = 0;

__attribute__((interrupt("WCH-Interrupt-fast"))) void SysTick_Handler(void)
{
    _timer_compare_func(_timer_compare_func_arg, SysTick_Handler);
}

static void timer_compare_set(uint64_t ticks, eer_callback_t *callback)
{

    _timer_compare_func     = (void (*)(void *, void *))callback->method;
    _timer_compare_func_arg = callback->argument;

    SysTick->CMP = ticks - 1; /* set reload register */
    eer_hw_isr_enable_system(SysTick_IRQn);

    SysTick->CTLR.w = 0xF;
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
    NVIC->IRER[((uint32_t)(SysTick_IRQn) >> 5)]
        = (1 << ((uint32_t)(SysTick_IRQn)&0x1F));
}

static uint16_t timer_ticks_to_us(uint16_t ticks) {
    return ticks / p_us;
}

eer_timer_handler_t eer_hw_timer = {.init        = timer_init,
                                    .get         = timer_get,
                                    .ticks_to_us = timer_ticks_to_us,
                                    .isr         = {
                                                .enable  = timer_isr_enable,
                                                .disable = timer_off,
                                    }};
