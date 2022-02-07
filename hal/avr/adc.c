#include "avr.h"

static void adc_mount(void *prescaler)
{
    // AREF = AVcc
    ADMUX = (1 << REFS0);

    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

static void adc_select_channel(void *channel)
{
    uint8_t ch = *(char *)channel;
    // select the corresponding channel 0~7
    // ANDing with ’7′ will always keep the value
    // of ‘ch’ between 0 and 7
    ch &= 0b00000111;            // AND operation with 7
    ADMUX = (ADMUX & 0xF8) | ch; // clears the bottom 3 bits before ORing
}

static void adc_start_convertion(void *channel)
{
    // start single convertion
    // write ’1′ to ADSC
    bit_set(ADCSRA, ADSC);
}

static bool adc_is_convertion_ready(void *channel)
{
    return (ADCSRA & (1 << ADSC)) == 0;
}

static uint16_t adc_read_convertion(void *channel) { return (ADC); }


eer_adc_handler_t eer_hw_adc = {
    .mount               = adc_mount,
    .select_channel      = adc_select_channel,
    .start_convertion    = adc_start_convertion,
    .is_convertion_ready = adc_is_convertion_ready,
    .read_convertion     = adc_read_convertion,
};

