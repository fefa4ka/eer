#include "avr.h"

static void timer_init(void *config)
{
    /* TCCR1B – Timer/Counter1 Control Register B
     * | CS12 | CS11 | CS10 | Description
     * |   0  |   1  |   0  | clkI/O/8 (from prescaler)
     * */
    TCCR1B |= (1 << CS11); // | (1 << WGM12);// | (1 << CS10);
}

static void *timer_get(void *value) { *(uint16_t)value = TCNT1; return value; }

static void (*_timer_compare_func)(void *trigger, void *argument);
static void *_timer_compare_func_arg = 0;

ISR(TIMER1_COMPA_vect)
{
    _timer_compare_func(_timer_compare_func_arg, TIMER1_COMPA_vect);
}

static void timer_compare_set(uint16_t ticks, eer_callback_t *callback)
{
    eer_hw_isr_disable();
    /* TIMSK1 – Timer/Counter1 Interrupt Mask Register
     * Bit    | 7 | 6 | 5     | 4 | 3 | 2      | 1      | 0     |
     * (0x6F) | – | – | ICIE1 | – | – | OCIE1B | OCIE1A | TOIE1 |
     */

    /* Bit 1 – OCIE1A: Timer/Counter1, Output Compare A Match Interrupt Enable
     * When this bit is written to one, and the I-flag in the status register
     * is set (interrupts globally enabled), the Timer/Counter1 output
     * compare A match interrupt is enabled. The corresponding interrupt vector
     * is executed when the OCF1A flag, located in TIFR1, is set.
     * */
    bit_clear(TIMSK1, OCIE1A); // Disable interupts

    _timer_compare_func     = callback->method;
    _timer_compare_func_arg = callback->argument;

    /* OCR1A - 16 bit output compare register
     * (0x89) OCR1AH Timer/Counter1 - Output compare register A high byte
     * (0x88) OCR1AL Timer/Counter1 - Output compare register A low byte
     */
    OCR1A = ticks;

    bit_set(TIMSK1, OCIE1A); // Enable interupts
    /* TIFR1 – Timer/Counter1 Interrupt Flag Register
     *
     * Bit 1 – OCF1A: Timer/Counter1, Output Compare A Match Flag
     * OCF1A is automatically cleared when the output compare match A
     * interrupt vector is executed. Alternatively, OCF1A can be
     * cleared by writing a logic one to its bit location.
     */
    bit_set(TIFR1, OCF1A);
    eer_hw_isr_enable();
}

void *timer_isr_enable(void *args_ptr, eer_callback_t *callback)
{
    struct eer_timer_isr *args = (struct eer_timer_isr *)args_ptr;
    if (args->event == TIMER_EVENT_COMPARE) {
        timer_compare_set(args->ticks, callback);

        return TIMER1_COMPA_vect;
    }
}

static void timer_off()
{
    TIMSK1 &= ~(1 << OCIE1A); // Disable interrupts
}

static uint16_t timer_ticks_to_us(uint16_t ticks) { return ticks >> 1; }

eer_timer_handler_t eer_hw_timer = {.init        = timer_init,
                                    .get         = timer_get,
                                    .ticks_to_us = timer_ticks_to_us,
                                    .isr         = {
                                        .enable  = timer_isr_enable,
                                        .disable = timer_off,
                                    }};
