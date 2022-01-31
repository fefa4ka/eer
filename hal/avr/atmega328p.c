#include "atmega328p.h"


void io_in(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_clear(*Pin->port.ddr, Pin->number);
    bit_set(*Pin->port.port, Pin->number);
}

void io_out(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_set(*Pin->port.ddr, Pin->number);
}


void io_on(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_set(*Pin->port.port, Pin->number);
}

void io_off(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_clear(*Pin->port.port, Pin->number);
}

void io_flip(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    bit_flip(*Pin->port.port, Pin->number);
}

void io_pullup(void *pin) { io_on(pin); }

bool io_get(void *pin)
{
    eer_pin_t *Pin = (eer_pin_t *)pin;

    return *(Pin->port.pin) & (1 << (Pin->number));
}

//// PORT.D.2 Intrrupt
//void (*_io_int0_func)(void *argument, void *trigger);
//void *_io_int0_func_arg = 0;
//
//ISR(INT0_vect) { _io_int0_func(_io_int0_func_arg, INT0_vect); }
//
//static inline
//void io_int0_set(enum io_event event, eer_callback_t *callback)
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
//void (*_io_int1_func)(void *argument, void *trigger);
//void *_io_int1_func_arg = 0;
//
//ISR(INT1_vect) { _io_int1_func(_io_int1_func_arg, INT1_vect); }
//
//static inline
//void io_int1_set(enum io_event event, eer_callback_t *callback)
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
//void (*_io_pcint7_func)(void *argument, void *trigger);
//void *_io_pcint7_func_arg = 0;
//
//ISR(PCINT7_vect) { _io_pcint7_func(_io_pcint7_func_arg, PCINT7_vect); }
//
//static inline
//void io_pcint7_set(uint8_t pin, eer_callback_t *callback)
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
//     * (SREG) is set (one), pin change interrupt 0 is enabled. Any change on any
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
//void (*_io_pcint14_func)(void *argument, void *trigger);
//void *_io_pcint14_func_arg = 0;
//
//ISR(PCINT14_vect) { _io_pcint14_func(_io_pcint14_func_arg, PCINT14_vect); }
//
//static inline
//void io_pcint14_set(uint8_t pin, eer_callback_t *callback)
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
//     * (SREG) is set (one), pin change interrupt 1 is enabled. Any change on any
//     * enabled PCINT14..8 pin will cause an interrupt. The corresponding
//     * interrupt of pin change interrupt request is executed from the PCI1
//     * interrupt vector. PCINT14..8 pins are enabled individually by the PCMSK1
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
//void (*_io_pcint23_func)(void *argument, void *trigger);
//void *_io_pcint23_func_arg = 0;
//
//ISR(PCINT23_vect) { _io_pcint14_func(_io_pcint23_func_arg, PCINT23_vect); }
//
//static inline
//void io_pcint23_set(uint8_t pin, eer_callback_t *callback)
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
//     * (SREG) is set (one), pin change interrupt 1 is enabled. Any change on any
//     * enabled PCINT23..16 pin will cause an interrupt. The corresponding
//     * interrupt of pin change interrupt request is executed from the PCI2
//     * interrupt vector. PCINT23..16 pins are enabled individually by the PCMSK2
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
//void *io_isr_enable(void *settings, eer_callback_t *callback)
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

void adc_mount(void *prescaler)
{
    // AREF = AVcc
    ADMUX = (1 << REFS0);

    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void adc_select_channel(void *channel)
{
    char ch = *(char *)channel;
    // char ch = 2;
    // select the corresponding channel 0~7
    // ANDing with ’7′ will always keep the value
    // of ‘ch’ between 0 and 7
    ch &= 0b00000111;            // AND operation with 7
    ADMUX = (ADMUX & 0xF8) | ch; // clears the bottom 3 bits before ORing
}

void adc_start_convertion(void *channel)
{
    // start single convertion
    // write ’1′ to ADSC
    bit_set(ADCSRA, ADSC);
}

bool adc_is_convertion_ready(void *channel)
{
    return (ADCSRA & (1 << ADSC)) == 0;
}

int adc_read_convertion(void *channel) { return (ADC); }


// UART
void uart_init(void *baudrate)
{
    uint16_t baud = hw_uart_baudrate(*(uint16_t *)baudrate);

    /* Set baud rate
     *
     * UBRRnL and UBRRnH – USART Baud Rate Registers
     *
     * Bit 11:0 – UBRR11:0: USART Baud Rate Register
     *
     * This is a 12-bit register which contains the USART baud rate.
     * The UBRRnH contains the four most significant bits, and the
     * UBRRnL contains the eight least significant bits of the USART
     * baud rate.
     * Ongoing transmissions by the transmitter and receiver will be
     * corrupted if the baud rate is changed. Writing UBRRnL will
     * trigger an immediate update of the baud rate prescaler.
     */
    UBRR0H = (uint8_t)(baud >> 8);
    UBRR0L = (uint8_t)baud;

    /* UCSRnA – USART Control and Status Register n A
     *
     * Bit 1 – U2Xn: Double the USART Transmission Speed
     *
     * This bit only has effect for the asynchronous operation.
     * Write this bit to zero when using synchronous operation.
     * Writing this bit to one will reduce the divisor of the baud rate
     * divider from 16 to 8 effectively doubling the transfer rate for
     * asynchronous communication.
     */
    bit_set(UCSR0A, U2X0);

    /* Enable receiver and transmitter
     *
     * UCSRnB – USART Control and Status Register n B
     *
     * Bit 4 – RXENn: Receiver Enable n
     *
     * Writing this bit to one enables the USART receiver. The receiver
     * will override normal port operation for the RxDn pin when enabled.
     * Disabling the receiver will flush the receive buffer invalidating
     * the FEn, DORn, and UPEn flags.
     *
     * Bit 3 – TXENn: Transmitter Enable n
     *
     * Writing this bit to one enables the USART transmitter.
     * The transmitter will override normal port operation for the TxDn pin
     * when enabled. The disabling of the transmitter (writing TXENn to zero)
     * will not become effective until ongoing and pending transmissions
     * are completed, i.e., when the transmit shift register and transmit
     * buffer register do not contain data to be transmitted.
     * When disabled, the transmitter will no longer override the TxDn port.
     */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    /* Set frame format: 8 data, 2 stop bit
     *
     * Bit 2 – UCSZn2: Character Size n
     *
     * The UCSZn2 bits combined with the UCSZn1:0 bit in UCSRnC sets
     * the number of data bits (character size) in a frame the receiver
     * and transmitter use.
     * ┌────────┬────────┬────────┐
     * │ UCSZn2 │ UCSZn1 │ UCSZn0 │
     * ├────────┼────────┼────────┤
     * │    0   │    1   │    1   │  8-bit
     * └────────┴────────┴────────┘
     */
    UCSR0C = (1 << UCSZ01) | (3 << UCSZ00);
}

bool uart_is_data_received() { return (UCSR0A & (1 << RXC0)); }

bool uart_is_transmit_ready() { return (UCSR0A & (1 << UDRE0)); }

void uart_transmit(uint8_t data) { UDR0 = data; }

uint8_t uart_receive() { return UDR0; }


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
void timer_init(void *config)
{
    /* TCCR1B – Timer/Counter1 Control Register B
     * | CS12 | CS11 | CS10 | Description
     * |   0  |   1  |   0  | clkI/O/8 (from prescaler)
     * */
    TCCR1B |= (1 << CS11); // | (1 << WGM12);// | (1 << CS10);
}

uint16_t timer_get() { return TCNT1; }

void (*_timer_compare_func)(void *trigger, void *argument);
void *_timer_compare_func_arg = 0;

ISR(TIMER1_COMPA_vect)
{
    _timer_compare_func(_timer_compare_func_arg, TIMER1_COMPA_vect);
}

void timer_compare_set(uint16_t ticks, eer_callback_t *callback)
{
    cli();
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

    _timer_compare_func      = callback->method;
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
    sei();
}

//void *timer_isr_enable(void *args_ptr, eer_callback_t *callback)
//{
//    struct timer_isr *args = (struct timer_isr *)args_ptr;
//    if (args->event == TIMER_EVENT_COMPARE) {
//        timer_compare_set(args->ticks, callback);
//
//        return TIMER1_COMPA_vect;
//    }
//}

void timer_off()
{
    TIMSK1 &= ~(1 << OCIE1A); // Disable interrupts
}

uint16_t timer_ticks_to_us(uint16_t ticks) { return ticks >> 1; }

eer_hal eer_hw = {.io    = {.in     = io_in,
                 .out    = io_out,
                 .on     = io_on,
                 .off    = io_off,
                 .flip   = io_flip,
                 .get    = io_get,
                 .pullup = io_pullup,
//                 .isr = {
//                    .enable = io_isr_enable,
//                 },
          },
          .adc   = {.mount             = adc_mount,
                  .select_channel     = adc_select_channel,
                  .start_convertion   = adc_start_convertion,
                  .is_convertion_ready = adc_is_convertion_ready,
                  .read_convertion    = adc_read_convertion},
          .uart  = {.init            = uart_init,
                   .is_data_received  = uart_is_data_received,
                   .is_transmit_ready = uart_is_transmit_ready,
                   .transmit        = uart_transmit,
                   .receive         = uart_receive},
          .timer = {.init        = timer_init,
                    .get         = timer_get,
                    .ticks_to_us = timer_ticks_to_us,
//                    .isr         = {
//                        .enable  = timer_isr_enable,
//                        .disable = timer_off,
//                    }
                    }};

