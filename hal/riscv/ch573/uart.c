#include "ch573.h"
#include "gpio.h"

#define UART_RX_PIN_NUMBER 8
#define UART_TX_PIN_NUMBER 9

/* UART0 */
#define R8_UART0_RBR                                                           \
    (*((uint16_ptr_t)0x40003008)) // RO, UART0 receiver buffer, receiving byte
#define R16_UART0_DL (*((uint16_ptr_t)0x4000300C)) // RW, UART0 divisor latch
#define R16_CLK_SYS_CFG                                                        \
    (*((uint16_ptr_t)0x40001008)) // RWA, system clock configuration, SAM
#define RB_CLK_SYS_MOD                                                         \
    0xC0 // RWA, system clock source mode: 00=divided from 32MHz, 01=divided
         // from PLL-480MHz, 10=directly from 32MHz, 11=directly from 32KHz
#define UART_FIFO_SIZE 8 // UART FIFO size (depth)
#define R8_UART0_THR                                                           \
    (*((uint8_ptr_t)0x40003008)) // WO, UART0 transmitter holding, transmittal
                                 // byte
#define R8_UART0_RFC                                                           \
    (*((uint8_ptr_t)0x4000300A)) // RO, UART0 receiver FIFO count
#define R8_UART0_TFC                                                           \
    (*((uint8_ptr_t)0x4000300B)) // RO, UART0 transmitter FIFO count
#define R8_UART0_FCR (*((uint8_ptr_t)0x40003002)) // RW, UART0 FIFO control
#define R8_UART0_LCR (*((uint8_ptr_t)0x40003003)) // RW, UART0 line control
#define R8_UART0_DIV                                                           \
    (*((uint8_ptr_t)0x4000300E)) // RW, UART0 pre-divisor latch byte, only low 7
                                 // bit, from 1 to 0/128
#define R8_UART0_IER   (*((uint8_ptr_t)0x40003001)) // RW, UART0 interrupt enable
#define UART_FCR       2
#define RB_FCR_FIFO_EN 0x01 // RW, UART FIFO enable
#define RB_FCR_RX_FIFO_CLR                                                     \
    0x02 // WZ, clear UART receiver FIFO, high action, auto clear
#define RB_FCR_TX_FIFO_CLR                                                     \
    0x04 // WZ, clear UART transmitter FIFO, high action, auto clear
#define RB_FCR_FIFO_TRIG                                                       \
    0xC0 // RW, UART receiver FIFO trigger level: 00-1byte, 01-2bytes,
         // 10-4bytes, 11-7bytes
#define UART_LCR 3
#define RB_LCR_WORD_SZ                                                         \
    0x03 // RW, UART word bit length: 00-5bit, 01-6bit, 10-7bit, 11-8bit
#define RB_LCR_STOP_BIT 0x04 // RW, UART stop bit length: 0-1bit, 1-2bit
#define RB_LCR_PAR_EN   0x08 // RW, UART parity enable
#define RB_LCR_PAR_MOD                                                         \
    0x30 // RW, UART parity mode: 00-odd, 01-even, 10-mark, 11-space
#define RB_LCR_BREAK_EN 0x40 // RW, UART break control enable
#define RB_LCR_DLAB     0x80 // RW, UART reserved bit
#define RB_LCR_GP_BIT   0x80 // RW, UART general purpose bit
#define UART_IER        1
#define RB_IER_RECV_RDY                                                        \
    0x01 // RW, UART interrupt enable for receiver data ready
#define RB_IER_THR_EMPTY 0x02 // RW, UART interrupt enable for THR empty
#define RB_IER_LINE_STAT                                                       \
    0x04 // RW, UART interrupt enable for receiver line status
#define RB_IER_MODEM_CHG                                                       \
    0x08                   // RW, UART0 interrupt enable for modem status change
#define RB_IER_DTR_EN 0x10 // RW, UART0 DTR/TNOW output pin enable
#define RB_IER_RTS_EN 0x20 // RW, UART0 RTS output pin enable
#define RB_IER_TXD_EN 0x40 // RW, UART TXD pin enable
#define RB_IER_RESET                                                           \
    0x80 // WZ, UART software reset control, high action, auto clear

#define uart_baudrate(baudrate) ((sys_clock() / 8UL / baudrate) + 5) / 10

static uint32_t sys_clock(void)
{
    uint16_t rev;

    rev = R16_CLK_SYS_CFG & 0xff;
    if ((rev & 0x40) == (0 << 6)) { // 32M½øÐÐ·ÖÆµ
        return (32000000 / (rev & 0x1f));
    } else if ((rev & RB_CLK_SYS_MOD) == (1 << 6)) { // PLL½øÐÐ·ÖÆµ
        return (480000000 / (rev & 0x1f));
    } else { // 32K×öÖ÷Æµ
        return (32000);
    }
}


static void uart_init(void *baudrate)
{
    uint16_t baud = (uint16_t)uart_baudrate(*(uint32_t *)baudrate);

    /* Configure serial port 1: First configure the IO port mode, then configure
     * the serial port */
    bit_set(R32_PB_OUT, UART_RX_PIN_NUMBER);
    /* TXD-configure push-pull output, pay attention to let the IO port
       output high level first */
    bit_clear(R32_PA_PD_DRV, UART_TX_PIN_NUMBER);
    bit_set(R32_PA_DIR, UART_TX_PIN_NUMBER);

    /* RXD —  Configuration pull-up input */
    bit_clear(R32_PA_PD_DRV, UART_RX_PIN_NUMBER);
    bit_set(R32_PA_DIR, UART_RX_PIN_NUMBER);

    /* Set baud rate */
    R16_UART0_DL = baud;

    /* R8_UART0_FCR - FIFO Control Register
     *
     * Bit 7:6 - RB_FCR_FIFO_TRIG: Receive FIFO interrupt and hardware flow
     * control trigger
     *
     * Point selection：
     * 00: 1 byte;
     * 01: 2 bytes；
     * 10: 4 bytes;
     * 11: 7 bytes.
     *
     * Used to set the interrupt and hardware flow control for receiving FIFOs
     * The trigger point of the system, for example: 10 corresponds to 4 bytes，
     * That is, the full 4 bytes are received to generate the received data
     * available Interrupt, and automatically invalidate when hardware flow
     * control is enabled RTS pin.
     *
     * Bit 2 — RB_FCR_TX_FIFO_CLR: Send FIFO data
     *
     * 1: Clear the data sent to the FIFO (excluding TSR)
     * 0: Do not empty the FIFO data to be sent.
     *
     * Bit 1 - RB_FCR_RX_FIFO_CLR: Receive FIFO data
     *
     * 1: Clear the data in the receive FIFO (excluding RSR);
     * 0: Do not clear the data in the receive FIFO.
     *
     * Bit 0 - RB_FCR_FIFO_EN: FIFO enable bits:
     *
     * 1: Enable 8-byte FIFO;
     * 0: Disable FIFO.
     * 16C450 compatible mode with FIFO disabled, equivalent to
     * There is only one byte in the FIFO (RECV_TG1=0, RECV_TG0=0, FIFO_EN=1),
     * it is recommended to enable.
     */
    R8_UART0_FCR
        = (2 << 6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;


    R8_UART0_LCR = RB_LCR_WORD_SZ;
    R8_UART0_IER = RB_IER_TXD_EN;
    R8_UART0_DIV = 1;
}

static bool uart_is_data_received() { return (R8_UART0_RFC); }

static bool uart_is_transmit_ready()
{
    return (R8_UART0_TFC != UART_FIFO_SIZE);
}

static void uart_transmit(uint8_t data) { R8_UART0_THR = data; }

static uint8_t uart_receive() { return R8_UART0_RBR; }


eer_serial_handler_t eer_hw_uart = {.init              = uart_init,
                                    .is_data_received  = uart_is_data_received,
                                    .is_transmit_ready = uart_is_transmit_ready,
                                    .transmit          = uart_transmit,
                                    .receive           = uart_receive};
