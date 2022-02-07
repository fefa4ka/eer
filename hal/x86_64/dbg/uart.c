#include "dbg.h"
#include <pthread.h>
#include <stdlib.h>

// UART
pthread_t uart_thread;
char      uart_received = 0;
void *    uart_receiver(void *ptr)
{
    while (true) {
        uart_received = fgetc(stdin);
    }
}
static void uart_init(void *baudrate)
{
    system("/bin/stty raw");
    pthread_create(&uart_thread, NULL, *uart_receiver, NULL);
    eer_profiler_count(uart_init);
}

static inline bool uart_is_data_received()
{
    // printf("UART isDataReceived\r\n");
    eer_profiler_count(uart_is_data_received);

    return uart_received != 0;
}

static inline bool uart_is_transmit_ready()
{

    eer_profiler_count(uart_is_data_received);
    return true;
}

static inline void uart_transmit(unsigned char data)
{
    eer_profiler_count(uart_transmit);
    putchar(data);
}

static inline unsigned char uart_receive()
{
    char c        = uart_received;
    uart_received = 0;
    eer_profiler_count(uart_receive);
    return c;
}

eer_serial_handler_t eer_hw_uart = {
    .init              = uart_init,
    .is_data_received  = uart_is_data_received,
    .is_transmit_ready = uart_is_transmit_ready,
    .transmit          = uart_transmit,
    .receive           = uart_receive,
};
