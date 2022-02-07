#include <avr/interrupt.h>
#include <avr/io.h>

#include <hal.h>

#define eer_hw_isr_enable()  sei()
#define eer_hw_isr_disable() cli()

#ifdef HAL_gpio
#include "gpio.h"
extern eer_gpio_handler_t eer_hw_gpio;
#endif
#ifdef HAL_timer
extern eer_timer_handler_t eer_hw_timer;
#endif
#ifdef HAL_adc
extern eer_adc_handler_t eer_hw_adc;
#endif
#ifdef HAL_uart
extern eer_serial_handler_t eer_hw_uart;
#endif
