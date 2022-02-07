#include "dbg.h"
#include <errno.h>
#include <hash.h>
#include <math.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

// Profiling
#ifdef PROFILING
    #include "profiler.h"
#else
    #define eer_info(...)
    #define eer_debug(...)
    #define eer_print(...)
    #define eer_profiler_count(...)
    #define eer_profiler_count_log(...)
#endif

// IO
#define MAX_TABLE_SIZE 10007 // Prime Number
pthread_mutex_t get_pin_lock;

static char  pin_index = 33;
unsigned int pins_index[128];
eer_pin_t *  pins_buffer[128];

struct hash_table pins = {
    .index = pins_index,
    .data  = (void **)&pins_buffer,
    .size  = 128,
    .used  = 0,
};

FILE *log_file;

FILE *vcd_file;
FILE *vcd_file_log;


void log_init()
{
    if (!log_file)
        log_file = fopen("profiler.log", "w");

    if (log_file == NULL) {
        printf("Error: can't open profiler.log file: %d %s", errno,
               strerror(errno));
        exit(1);
    }
}

void log_printf(char *message, ...)
{
    va_list args;
    va_start(args, message);
    vfprintf(log_file, message, args);
    va_end(args);
}

void log_clean() { fclose(log_file); }

#define vcd_open(filename) fopen(#filename ".vcd", "w")
void vcd_init()
{
    if (vcd_file && vcd_file_log)
        return;

    // use appropriate location if you are using MacOS or Linux
    vcd_file     = vcd_open(profiler);
    vcd_file_log = vcd_open(profiler_log);

    if (vcd_file == NULL) {
        printf("Error: can't open vcd file");
        exit(1);
    }

    if (vcd_file_log == NULL) {
        printf("Error: can't open vcd file");
        exit(1);
    }
}

void vcd_clean()
{
    char c;
    fprintf(vcd_file, "$date"
                      "\n"
                      "   October 17, 2021."
                      "\n"
                      "$end"
                      "\n"
                      "$version"
                      "\n"
                      "   VCD generator tool version info text."
                      "\n"
                      "$end"
                      "\n"
                      "$comment"
                      "\n"
                      "   Any comment text."
                      "\n"
                      "$end"
                      "\n"
                      "$timescale 1us $end"
                      "\n"
                      "$scope module logic $end\n");
    for (unsigned int index = 0; index < pins.used; index++) {
        eer_pin_t *Pin = pins.data[index];
        fprintf(vcd_file, "$var wire 1 %c %s_%d$end\n", Pin->index, Pin->name,
                Pin->number);
    }

    fprintf(vcd_file, "$upscope $end"
                      "\n"
                      "$enddefinitions $end"
                      "\n");
    fclose(vcd_file_log);
    vcd_file_log = fopen("profiler_log.vcd", "r");
    if (vcd_file_log) {
        while ((c = fgetc(vcd_file_log)) != EOF)
            fputc(c, vcd_file);
        fclose(vcd_file_log);
        remove("profiler_log.vcd");
    } else {
        printf("Error on profiler_log.vcd generation: %d %s\n", errno,
               strerror(errno));
    }
    fclose(vcd_file);
}

unsigned int hash_pin(eer_pin_t *pin)
{
    unsigned long hash = 0;
    char *        word = pin->name;

    while (*word != '\0') {
        hash = *word++ + 31 * hash;
    }
    hash += pin->number;

    // log_info("%s_%d=%d", pin->name, pin->number, hash % MAX_TABLE_SIZE);
    return (hash % MAX_TABLE_SIZE);
}

void gpio_thread() {}
void gpio_init()
{
    log_init();
    vcd_init();
    pthread_mutex_init(&get_pin_lock, NULL);
    // TODO: thread with gpio manipulation
}

static eer_pin_t *get_pin(eer_pin_t *pin)
{
    eer_pin_t *Pin;
    pthread_mutex_lock(&get_pin_lock);
    if (hash_read(&pins, hash_pin(pin), (void **)&Pin) == OK) {
        pthread_mutex_unlock(&get_pin_lock);
        // printf("Pin PORT_%s_%d — cached\r\n", Pin->name, Pin->number);
        return Pin;
    }

    Pin            = malloc(sizeof(eer_pin_t));
    Pin->name      = strdup(pin->name);
    Pin->number    = pin->number;
    Pin->port.port = 0;
    Pin->port.pin  = 0;
    Pin->port.ddr  = 0;

    // printf("%s - %d - %d\n", pin->name, hash_pin(pin), hash_pin(Pin));
    hash_write(&pins, hash_pin(Pin), Pin);

    Pin->index = pin_index++;
    // printf("Pin PORT_%s_%d = (#%d %x) new\r\n", Pin->name, Pin->number,
    // hash_pin(pin), &(Pin->port));
    pthread_mutex_unlock(&get_pin_lock);
    return Pin;
}

static bool gpio_get(void *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);

    eer_profiler_count_log(gpio_get, "(%s_%d)=%d", Pin->name, Pin->number,
                           (Pin->port.pin) & (1 << Pin->number));

    return (Pin->port.pin) & (1 << Pin->number);
}

void dump_pin(eer_pin_t *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);
    printf("Pin PORT_%s_%d #%d = %d\n", Pin->name, Pin->number, hash_pin(pin),
           gpio_get(pin));
}

void free_pins()
{
    for (unsigned int index = 0; index < pins.used; index++) {
        eer_pin_t *Pin = pins.data[index];
        free(Pin->name);
        free(Pin);
    }
    pthread_mutex_destroy(&get_pin_lock);
#ifdef PROFILING
    vcd_clean();
    log_clean();
#endif
    system("/bin/stty cooked");
}

static void gpio_in(void *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);

    bit_clear(Pin->port.ddr, Pin->number);
    bit_set(Pin->port.port, Pin->number);

    eer_profiler_count_log(gpio_in, "(%s_%d)", Pin->name, Pin->number);
    // printf("Pin PORT_%s_%d — in\r\n", Pin->port, Pin->number);
}

static void gpio_out(void *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);

    bit_set(Pin->port.ddr, Pin->number);

    eer_profiler_count_log(gpio_out, "(%s_%d)", Pin->name, Pin->number);
    // printf("Pin PORT_%s_%d — out\r\n", Pin->port, Pin->number);
}

static void gpio_on(void *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);

    clock_t current_time = clock();
    if (!((Pin->port.pin) & (1 << Pin->number))) {
        fprintf(vcd_file_log, "#%ld\n%d%c\n", current_time, 1, Pin->index);
    }
    bit_set(Pin->port.pin, Pin->number);

    // printf("On %x ", &(Pin->port));
    // dump_pin(pin);
    // printf("Pin PORT_%s_%d — on - %d - %d\r\n", Pin->name, Pin->number,
    // Pin->port.port, 1 << Pin->number);
    eer_profiler_count_log(gpio_on, "(%s_%d)", Pin->name, Pin->number);
}

static void gpio_off(void *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);

    clock_t current_time = clock();
    if (((Pin->port.pin) & (1 << Pin->number))) {
        fprintf(vcd_file_log, "#%ld\n%d%c\n", current_time, 0, Pin->index);
    }
    bit_clear(Pin->port.pin, Pin->number);

    // printf("Off %x ", &(Pin->port));
    // dump_pin(pin);
    // printf("Pin PORT_%s_%d — off - %d\r\n", Pin->name, Pin->number,
    // Pin->port.port);
    eer_profiler_count_log(gpio_off, "(%s_%d)", Pin->name, Pin->number);
}

static void gpio_flip(void *pin)
{
    eer_pin_t *Pin          = get_pin((eer_pin_t *)pin);
    clock_t    current_time = clock();

    bit_flip(Pin->port.pin, Pin->number);
    fprintf(vcd_file_log, "#%ld\n%d%c\n", current_time, gpio_get(pin),
            Pin->index);
    eer_profiler_count_log(gpio_flip, "(%s_%d)", Pin->name, Pin->number);
}

static void gpio_pullup(void *pin)
{
    eer_pin_t *Pin = get_pin((eer_pin_t *)pin);

    bit_set(Pin->port.pin, Pin->number);
    eer_profiler_count_log(gpio_pullup, "(%s_%d)", Pin->name, Pin->number);
}


// ADC

static void adc_mount(void *prescaler)
{
    srand(time(NULL));
    // printf("ADC init\r\n");
    eer_profiler_count(adc_mount);
}

static void adc_select_channel(void *channel)
{
    // unsigned short *ch = (unsigned short *)channel;

    // printf("ADC selectChannel %d\r\n", *ch);
    eer_profiler_count(adc_select_channel);
}

static void adc_start_convertion(void *channel)
{
    // printf("ADC startConvertion\r\n");
    eer_profiler_count(adc_start_convertion);
}

static bool adc_is_convertion_ready(void *channel)
{
    // unsigned short *ch = (unsigned short *)channel;
    // printf("ADC isConvertionReady %d\r\n", *ch);

    eer_profiler_count(adc_is_convertion_ready);
    return true;
}

static int16_t adc_read_convertion(void *channel)
{
    // unsigned short *ch = (unsigned short *)channel;
    // printf("ADC readConvertion %d\r\n", *ch);

    eer_profiler_count(adc_read_convertion);
    return sin(clock() / 1e6) * 100;
}


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

/* Time */
static void timer_init(void *config)
{
    eer_profiler_count(timer_init);
    srand(time(NULL));
}

long          tick = 0;
unsigned long time_in_ns()
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1e9 + now.tv_nsec;
}

unsigned long timer_get_ns()
{
    long tock   = time_in_ns();
    long passed = tock - tick;

    if (tick) {
        tick = tock;
        return passed;
    }

    tick = tock;
    return 0;
}

static inline uint16_t timer_get()
{
    clock_t tick = clock();
    // printf("%ld - %ld\n", tick, timer_get_ns());
    eer_profiler_count_log(timer_get, "()=%d", tick);
    return tick;
    // printf("Timer get: %ld, %ld, %d\r\n", tick, CLOCKS_PER_SEC, tick /
    // CLOCKS_PER_SEC);
}

struct timer_callback {
    unsigned int timeout;
    eer_result_t (*callback)(void *argument, void *trigger);
    void *    args;
    size_t    index;
    pthread_t thread;
};

#define TIMERS_NR 10
struct timer_callback timer_callback_buffer[TIMERS_NR] = {0};

void *timer_timeout(void *ptr)
{
    struct timer_callback *callback = ptr;

    usleep(callback->timeout);
    callback->callback(callback->args, 0);
    *callback = (struct timer_callback){0};

    return NULL;
}

static void timer_compare_set(uint16_t ticks, eer_callback_t *callback)
{
    eer_profiler_count(timer_compare_set);
    for (size_t i = 0; i < TIMERS_NR; i++) {
        if (timer_callback_buffer[i].callback == NULL) {
            struct timer_callback timer_callback
                = {ticks, callback->method, callback->argument, i};
            struct timer_callback *timer = timer_callback_buffer + i;
            *timer                       = timer_callback;
            pthread_create(&timer->thread, NULL, *timer_timeout, (void *)timer);

            // printf("Timer #%ld set: %d\r\n", i, ticks);
            break;
        }
    }
}

void *timer_isr_enable(void *args_ptr, eer_callback_t *callback)
{
    struct eer_timer_isr *args = (struct eer_timer_isr *)args_ptr;
    if (args->event == TIMER_EVENT_COMPARE) {
        timer_compare_set(args->ticks, callback);

        return timer_compare_set;
    }

    return 0;
}


static void timer_off()
{
    eer_profiler_count(timer_off);
    // printf("Timer off\r\n");
}

static uint16_t timer_ticks_to_us(uint16_t ticks)
{
    eer_profiler_count_log(timer_ticks_to_us, "(%d)=%d", ticks, ticks);
    return ticks / 1;
}

eer_gpio_handler_t eer_hw_gpio = {
    .in     = gpio_in,
    .out    = gpio_out,
    .on     = gpio_on,
    .off    = gpio_off,
    .flip   = gpio_flip,
    .get    = gpio_get,
    .pullup = gpio_pullup,
};

eer_adc_handler_t eer_hw_adc = {
    .mount               = adc_mount,
    .select_channel      = adc_select_channel,
    .start_convertion    = adc_start_convertion,
    .is_convertion_ready = adc_is_convertion_ready,
    .read_convertion     = adc_read_convertion,
};
eer_serial_handler_t eer_hw_uart = {
    .init              = uart_init,
    .is_data_received  = uart_is_data_received,
    .is_transmit_ready = uart_is_transmit_ready,
    .transmit          = uart_transmit,
    .receive           = uart_receive,
};

eer_timer_handler_t eer_hw_timer = {.init        = timer_init,
                                    .get         = timer_get,
                                    .ticks_to_us = timer_ticks_to_us,
                                    .isr         = {
                                        .enable  = timer_isr_enable,
                                        .disable = timer_off,
                                    }};

// Returns the local date/time formatted as 2014-03-19 11:11:52.132
char *get_formatted_time(void)
{
    // Must be static, otherwise won't work
    static char _retval[37];
    char        _strtime[20];

    time_t         rawtime;
    struct tm *    timeinfo;
    struct timeval tv;

    int millisec;

    time(&rawtime);
    gettimeofday(&tv, NULL);
    timeinfo = localtime(&rawtime);

    millisec = lrint(tv.tv_usec / 1000.0);
    if (millisec >= 1000) {
        millisec -= 1000;
        tv.tv_sec++;
    }

    strftime(_strtime, sizeof(_strtime), "%Y-%m-%d %H:%M:%S", timeinfo);
    sprintf(_retval, "%s.%03d", _strtime, millisec);

    return _retval;
}
