#include "avr.h"

#define uart_baudrate(baudrate)                                                \
    (((F_CPU) + 4UL * (baudrate)) / (8UL * (baudrate)) - 1UL)

static void uart_init(void *baudrate)
{
    uint16_t baud = uart_baudrate(*(uint16_t *)baudrate);

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

static bool uart_is_data_received() { return (UCSR0A & (1 << RXC0)); }

static bool uart_is_transmit_ready() { return (UCSR0A & (1 << UDRE0)); }

static void uart_transmit(uint8_t data) { UDR0 = data; }

static uint8_t uart_receive() { return UDR0; }


eer_serial_handler_t eer_hw_uart = {.init              = uart_init,
                                    .is_data_received  = uart_is_data_received,
                                    .is_transmit_ready = uart_is_transmit_ready,
                                    .transmit          = uart_transmit,
                                    .receive           = uart_receive};
