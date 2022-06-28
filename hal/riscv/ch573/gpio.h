#pragma once

#include "ch573.h"


/* GPIO PA register */
#define R32_PA_DIR                                                             \
    (*((uint32_ptr_t)0x400010A0)) // RW, GPIO PA I/O direction: 0=in, 1=out
#define R32_PA_PIN (*((uint32_ptr_t)0x400010A4)) // RO, GPIO PA input
#define R32_PA_OUT (*((uint32_ptr_t)0x400010A8)) // RW, GPIO PA output
#define R32_PA_CLR                                                             \
    (*((uint32_ptr_t)0x400010AC)) // WZ, GPIO PA clear output: 0=keep, 1=clear
#define R32_PA_PU                                                              \
    (*((uint32_ptr_t)0x400010B0)) // RW, GPIO PA pullup resistance enable
#define R32_PA_PD_DRV                                                          \
    (*((uint32_ptr_t)0x400010B4)) // RW, PA pulldown for input or PA driving
                                  // capability for output

/* GPIO PB register */
#define R32_PB_DIR                                                             \
    (*((uint32_ptr_t)0x400010C0)) // RW, GPIO PB I/O direction: 0=in, 1=out
#define R32_PB_PIN (*((uint32_ptr_t)0x400010C4)) // RO, GPIO PB input
#define R32_PB_OUT (*((uint32_ptr_t)0x400010C8)) // RW, GPIO PB output
#define R32_PB_CLR                                                             \
    (*((uint32_ptr_t)0x400010CC)) // WZ, GPIO PB clear output: 0=keep, 1=clear
#define R32_PB_PU                                                              \
    (*((uint32_ptr_t)0x400010D0)) // RW, GPIO PB pullup resistance enable
#define R32_PB_PD_DRV                                                          \
    (*((uint32_ptr_t)0x400010D4)) // RW, PB pulldown for input or PB driving
                                  // capability for output


typedef struct {
    struct {
        uint32_ptr_t out; /* RW, GPIO PA output */
        uint32_ptr_t pin; /* RO, GPIO PA input */
        uint32_ptr_t drv; /* RW, PA pulldown for input or PA driving
                                capability for output */
        uint32_ptr_t pu;  /* RW, GPIO PA pullup resistance enable */
        uint32_ptr_t dir; /* RW, GPIO PA I/O direction: 0=in, 1=out */
        uint32_ptr_t clr; /* WZ, GPIO PA clear output: 0=keep, 1=clear */
    } port;
    uint32_t          number;
    enum eer_pin_mode mode;
} eer_pin_t;


#define eer_hw_pin(port, pin)                                                  \
    {                                                                          \
        {                                                                      \
            &R32_P##port##_OUT, &R32_P##port##_PIN, &R32_P##port##_PD_DRV,        \
            &R32_P##port##_PU,  &R32_P##port##_DIR, &R32_P##port##_CLR,     \
        },                                                                     \
            pin,                                                               \
        {                                                                      \
            0                                                                  \
        }                                                                      \
    }
