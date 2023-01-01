#pragma once

#include "ch573.h"

#define R8_SAFE_ACCESS_SIG                                                     \
    (*((uint8_ptr_t)0x40001040)) // WO, safe accessing sign register, must write
                                 // SAFE_ACCESS_SIG1 then SAFE_ACCESS_SIG2 to
                                 // enter safe accessing mode

#define R8_CK32K_CONFIG     (*((uint8_ptr_t)0x4000102F))  // RWA, 32KHz oscillator configure
#define  RB_CLK_XT32K_PON   0x01                      // RWA, external 32KHz oscillator power on
#define  RB_CLK_INT32K_PON  0x02                      // RWA, internal 32KHz oscillator power on
#define  RB_CLK_OSC32K_XT   0x04                      // RWA, 32KHz oscillator source selection: 0=RC, 1=XT
#define  RB_32K_CLK_PIN     0x80                      // RO, 32KHz oscillator clock pin status

enum ch573_clock_source
{
  CLK_SOURCE_LSI = 0x00,
  CLK_SOURCE_LSE,

  CLK_SOURCE_HSE_16MHz = 0x22,
  CLK_SOURCE_HSE_8MHz = 0x24,
  CLK_SOURCE_HSE_6_4MHz = 0x25,
  CLK_SOURCE_HSE_4MHz = 0x28,
  CLK_SOURCE_HSE_2MHz = (0x20|16),
  CLK_SOURCE_HSE_1MHz = (0x20|0),

  CLK_SOURCE_PLL_60MHz = 0x48,
  CLK_SOURCE_PLL_48MHz = (0x40|10),
  CLK_SOURCE_PLL_40MHz = (0x40|12),
  CLK_SOURCE_PLL_36_9MHz = (0x40|13),
  CLK_SOURCE_PLL_32MHz = (0x40|15),
  CLK_SOURCE_PLL_30MHz = (0x40|16),
  CLK_SOURCE_PLL_24MHz = (0x40|20),
  CLK_SOURCE_PLL_20MHz = (0x40|24),
  CLK_SOURCE_PLL_15MHz = (0x40|0),
};

typedef enum ch573_clock_source eer_sys_clk_source_t;
typedef enum ch573_clock_source eer_sys_rtc_source_t;
