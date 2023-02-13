#pragma once

#include "ch32v003.h"

/* Reset and Clock Control */
/* Clock frequency expressed in Hz */
typedef struct
{
    uint32_t SYSCLK_Frequency;
    uint32_t HCLK_Frequency;
    uint32_t PCLK1_Frequency;
    uint32_t PCLK2_Frequency;
    uint32_t ADCCLK_Frequency;
} RCC_ClocksTypeDef;


enum ch32v003_clock_source {
    SYSCLK_FREQ_8MHz_HSI = 1,
    SYSCLK_FREQ_24MHZ_HSI,
    SYSCLK_FREQ_48MHZ_HSI,
    SYSCLK_FREQ_8MHz_HSE,
    SYSCLK_FREQ_24MHz_HSE,
    SYSCLK_FREQ_48MHz_HSE
};

typedef enum ch32v003_clock_source eer_sys_clk_source_t;
typedef enum ch32v003_clock_source eer_sys_rtc_source_t;


