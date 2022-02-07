#include "atmega328p.h"


//// PORT.D.2 Intrrupt
// void (*_io_int0_func)(void *argument, void *trigger);
// void *_io_int0_func_arg = 0;
//
// ISR(INT0_vect) { _io_int0_func(_io_int0_func_arg, INT0_vect); }
//
// static inline
// void io_int0_set(enum io_event event, eer_callback_t *callback)
//{
//    cli();
//    // EIMSK – External Interrupt Mask Register
//    bit_clear(EIMSK, INT0); // Disable INT0 interrupt
//
//    bit_clear(DDRD, 2); // PD2 as input (INT0)
//    bit_set(PORTD, 2);  // pull-up resistor to PD2 (INT0)
//
//    _io_int0_func     = callback->method;
//    _io_int0_func_arg = callback->argument;
//
//    /* EICRA – External Interrupt Control Register A
//     * | ISC01 | ISC00 | Description */
//    if (event == IO_EVENT_RISE) {
//        /* | 1 | 1 | The rising edge of INT0 generates an interrupt */
//        bit_set(EICRA, ISC00);
//        bit_set(EICRA, ISC01);
//    } else if (event == IO_EVENT_FALL)
//        /* | 1 | 0 | The falling edge of INT0 generates an interrupt */
//        bit_set(EICRA, ISC01);
//    else
//        /* | 0 | 1 | Any logical change on INT0 generates an interrupt */
//        bit_set(EICRA, ISC00);
//
//    // TODO: * | 0 | 0 | The low level of INT0 generates an interrupt
//
//    bit_set(EIMSK, INT0); // Enable INT0 interrupt
//
//    sei();
//}
//
//// PORT.D.3 Interrupt
// void (*_io_int1_func)(void *argument, void *trigger);
// void *_io_int1_func_arg = 0;
//
// ISR(INT1_vect) { _io_int1_func(_io_int1_func_arg, INT1_vect); }
//
// static inline
// void io_int1_set(enum io_event event, eer_callback_t *callback)
//{
//    cli();
//    /* EIMSK – External Interrupt Mask Register
//     * Disable INT1 interrupt */
//    bit_clear(EIMSK, INT1);
//
//    /* PD3 as input (INT1) */
//    bit_clear(DDRD, 3);
//    /* Pull-up resistor to PD3 (INT1) */
//    bit_set(PORTD, 3);
//
//    _io_int1_func     = callback->method;
//    _io_int1_func_arg = callback->argument;
//
//    /* EICRA – External Interrupt Control Register A
//     * | ISC01 | ISC00 | Description */
//    if (event == IO_EVENT_RISE) {
//        /* | 1 | 1 | The rising edge of INT0 generates an interrupt */
//        bit_set(EICRA, ISC10);
//        bit_set(EICRA, ISC11);
//    } else if (event == IO_EVENT_FALL)
//        /* | 1 | 0 | The falling edge of INT0 generates an interrupt */
//        bit_set(EICRA, ISC11);
//    else
//        /* | 0 | 1 | Any logical change on INT0 generates an interrupt */
//        bit_set(EICRA, ISC10);
//
//    // TODO: * | 0 | 0 | The low level of INT0 generates an interrupt
//
//    bit_set(EIMSK, INT1); // Enable INT0 interrupt
//
//    sei();
//}
//
///* PCINT7..0 */
// void (*_io_pcint7_func)(void *argument, void *trigger);
// void *_io_pcint7_func_arg = 0;
//
// ISR(PCINT7_vect) { _io_pcint7_func(_io_pcint7_func_arg, PCINT7_vect); }
//
// static inline
// void io_pcint7_set(uint8_t pin, eer_callback_t *callback)
//{
//    cli();
//
//    _io_pcint7_func     = callback->method;
//    _io_pcint7_func_arg = callback->argument;
//
//    /* PCICR – Pin Change Interrupt Control Register
//     *
//     * Bit 0 - PCIE0: Pin Change Interrupt Enable 0
//     *
//     * When the PCIE0 bit is set (one) and the I-bit in the status register
//     * (SREG) is set (one), pin change interrupt 0 is enabled. Any change on
//     any
//     * enabled PCINT7..0 pin will cause an interrupt. The corresponding
//     * interrupt of pin change interrupt request is executed from the PCI0
//     * interrupt vector. PCINT7..0 pins are enabled individually by the PCMSK0
//     * register.
//     */
//    bit_set(PCICR, PCIE0);
//
//    /* PCMSK0 – Pin Change Mask Register 0
//     *
//     * Bit 7..0 – PCINT7..0: Pin Change Enable Mask 7..0
//     *
//     * Each PCINT7..0 bit selects whether pin change interrupt
//     * is enabled on the corresponding I/O pin. If PCINT7..0
//     * is set and the PCIE0 bit in PCICR is set, pin change interrupt
//     * is enabled on the corresponding I/O pin. If PCINT7..0 is cleared,
//     * pin change interrupt on the corresponding I/O pin is disabled.
//     */
//    bit_set(PCMSK0, pin);
//
//    sei();
//}
//
///* PCINT14..8 */
// void (*_io_pcint14_func)(void *argument, void *trigger);
// void *_io_pcint14_func_arg = 0;
//
// ISR(PCINT14_vect) { _io_pcint14_func(_io_pcint14_func_arg, PCINT14_vect); }
//
// static inline
// void io_pcint14_set(uint8_t pin, eer_callback_t *callback)
//{
//    cli();
//
//    _io_pcint14_func     = callback->method;
//    _io_pcint14_func_arg = callback->argument;
//
//    /* PCICR – Pin Change Interrupt Control Register
//     *
//     * Bit 1 - PCIE1: Pin Change Interrupt Enable 1
//     *
//     * When the PCIE0 bit is set (one) and the I-bit in the status register
//     * (SREG) is set (one), pin change interrupt 1 is enabled. Any change on
//     any
//     * enabled PCINT14..8 pin will cause an interrupt. The corresponding
//     * interrupt of pin change interrupt request is executed from the PCI1
//     * interrupt vector. PCINT14..8 pins are enabled individually by the
//     PCMSK1
//     * register.
//     */
//    bit_set(PCICR, PCIE1);
//
//    /* PCMSK1 – Pin Change Mask Register 1
//     *
//     * Bit 14..8 – PCINT14..8: Pin Change Enable Mask 14..8
//     *
//     * is enabled on the corresponding I/O pin. If PCINT14..8
//     * is set and the PCIE1 bit in PCICR is set, pin change interrupt
//     * is enabled on the corresponding I/O pin. If PCINT14..8 is cleared,
//     * pin change interrupt on the corresponding I/O pin is disabled.
//     */
//    bit_set(PCMSK1, pin);
//
//    sei();
//}
//
///* PCINT23..16 */
// void (*_io_pcint23_func)(void *argument, void *trigger);
// void *_io_pcint23_func_arg = 0;
//
// ISR(PCINT23_vect) { _io_pcint14_func(_io_pcint23_func_arg, PCINT23_vect); }
//
// static inline
// void io_pcint23_set(uint8_t pin, eer_callback_t *callback)
//{
//    cli();
//
//    _io_pcint23_func     = callback->method;
//    _io_pcint23_func_arg = callback->argument;
//
//    /* PCICR – Pin Change Interrupt Control Register
//     *
//     * Bit 2 - PCIE2: Pin Change Interrupt Enable 2
//     *
//     * When the PCIE2 bit is set (one) and the I-bit in the status register
//     * (SREG) is set (one), pin change interrupt 1 is enabled. Any change on
//     any
//     * enabled PCINT23..16 pin will cause an interrupt. The corresponding
//     * interrupt of pin change interrupt request is executed from the PCI2
//     * interrupt vector. PCINT23..16 pins are enabled individually by the
//     PCMSK2
//     * register.
//     */
//    bit_set(PCICR, PCIE2);
//
//    /* PCMSK2 – Pin Change Mask Register 2
//     *
//     * Bit 23..16 – PCINT23..16: Pin Change Enable Mask 23..16
//     *
//     * Each PCINT23..16 bit selects whether pin change interrupt
//     * is enabled on the corresponding I/O pin. If PCINT23..16
//     * is set and the PCIE2 bit in PCICR is set, pin change interrupt
//     * is enabled on the corresponding I/O pin. If PCINT23..16 is cleared,
//     * pin change interrupt on the corresponding I/O pin is disabled.
//     */
//    bit_set(PCMSK2, pin);
//
//    sei();
//}
//
//
///* Enabled interrupt, enable pin */
///* When mount for different ports one interrupt we need list of vectros */
///* Dispatcher needed for each ISR 8 pointers */
///* Save index as high byte use port and low pin */
// void *io_isr_enable(void *settings, eer_callback_t *callback)
//{
//    struct io_isr *isr = (struct io_isr *)settings;
//    eer_pin_t *        Pin = isr->pin;
//
//    if (Pin->port.pin == &PIND) {
//        if (Pin->number == 2) {
//            io_int0_set(isr->event, callback);
//
//            return INT0_vect;
//        } else if (Pin->number == 3) {
//            io_int1_set(isr->event, callback);
//
//            return INT1_vect;
//        } else {
//            io_pcint23_set(Pin->number, callback);
//
//            return PCINT23_vect;
//        }
//    } else if (Pin->port.pin == &PINB) {
//        io_pcint7_set(Pin->number, callback);
//
//        return PCINT7_vect;
//    } else if (Pin->port.pin == &PINC) {
//        io_pcint14_set(Pin->number, callback);
//
//        return PCINT14_vect;
//    }
//}
//
// TIDI
/*
#define HW_ISR_VECTOR_SIZE 8
unsigned int isr_index[HW_ISR_VECTOR_SIZE];
eer_callback_t isr_handlers[HW_ISR_VECTOR_SIZE];

union isr_vector_index {
    struct {
        char port;
        char num;
    } pin;
    unsigned int index;
};

struct hash_table isr_vectors = {
    .index = isr_index,
    .data = (void **)&isr_handlers,
    .size = HW_ISR_VECTOR_SIZE
};

void
io_isr_handler(char vector) {
     for(unsigned int index = 0; index <= isr_vectors.used; index++) {
        union isr_vector_index vector_index;
        vector_index.index = isr_vectors.index[index];
        if((vector_index.pin.port ^ vector) == 0) {
            eer_callback_t *isr_handler = isr_vectors.data[index];
            isr_handler->method(0, isr_handler->argument);
        }
    }
}
*/


/*
 *  The pin change interrupt PCI1 will trigger if any enabled PCINT14..8 pin
 * toggles. The pin change interrupt PCI0 will trigger if any enabled PCINT7..0
 * pin toggles.
 */
// void io_isr_mount(void *pin, eer_callback_t *callback)
//{
//    eer_pin_t *Pin = (eer_pin_t *)pin;
//    union isr_vector_index vector_index = { .pin = { .num = 2 } };
//    /* INT0 */
//    if(Pin->port.pin == &PIND && Pin->number == 2) {
//        vector_index.pin.port = 'A';
//    } else if() {
//    }

//    hash_write(&isr_vectors, vector_index.index, callback);
/* INT1 */
//}


// ADC


// SPI
#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_MISO PORTB4
#define SPI_MOSI PORTB3
#define SPI_SCK  PORTB5
#define SPI_SS   PORTB2

#define SPI_SPCR SPCR
#define SPI_SPDR SPDR
#define SPI_SPSR SPSR
#define SPI_SPIF SPIF
#define SPI_SPE  SPE
#define SPI_MSTR MSTR
#define SPI_SPR0 SPR0
#define SPI_SPR1 SPR1
void spi_init(void *config)
{
    // make the MOSI, SCK, and SS pins outputs
    SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS);

    // make sure the MISO pin is input
    SPI_DDR &= ~(1 << SPI_MISO);

    // set up the SPI module: SPI enabled, MSB first, master mode,
    //  clock polarity and phase = 0, F_osc/16
    SPI_SPCR = (1 << SPI_SPE) | (1 << SPI_MSTR); // | ( 1 << SPI_SPR0 );
    SPI_SPSR = 1; // set double SPI speed for F_osc/2
}

bool spi_is_data_received()
{
    if (!(SPI_SPSR & (1 << SPI_SPIF))) {
        SPI_SPDR = 0xFF;
    } else {
        return true;
    }
}

bool spi_is_transmit_ready() { return !(SPI_SPSR & (1 << SPI_SPIF)); }

void spi_transmit(unsigned char data) { SPI_SPDR = data; }

unsigned char spi_receive() { return SPI_SPDR; }


/* Timer */

eer_hal eer_hw = {
    .gpio  = &eer_hw_gpio,
    .adc   = &eer_hw_adc,
    .uart  = &eer_hw_uart,
    .timer = &eer_hw_timer,
};
