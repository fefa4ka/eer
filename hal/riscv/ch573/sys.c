#include "sys.h"

#define R16_CLK_SYS_CFG                                                        \
    (*((uint16_ptr_t)0x40001008)) // RWA, system clock configuration, SAM
#define RB_CLK_SYS_MOD                                                         \
    0xC0 // RWA, system clock source mode: 00=divided from 32MHz, 01=divided
         // from PLL-480MHz, 10=directly from 32MHz, 11=directly from 32KHz


/* System: safe accessing register */
#define R32_SAFE_ACCESS (*((uint32_ptr_t)0x40001040)) // RW, safe accessing

#define RB_SAFE_ACC_MODE                                                       \
    0x03 // RO, current safe accessing mode: 11=safe/unlocked (SAM),
         // other=locked (00..01..10..11)
#define RB_SAFE_ACC_ACT                                                        \
    0x08 // RO, indicate safe accessing status now: 0=locked, read only,
         // 1=safe/unlocked (SAM), write enabled
#define RB_SAFE_ACC_TIMER                                                      \
    0x70 // RO, safe accessing timer bit mask (16*clock number)
#define SAFE_ACCESS_SIG1 0x57 // WO: safe accessing sign value step 1
#define SAFE_ACCESS_SIG2 0xA8 // WO: safe accessing sign value step 2
#define SAFE_ACCESS_SIG0 0x00 // WO: safe accessing sign value for disable
#define R8_CHIP_ID                                                             \
    (*((uint8_ptr_t)0x40001041)) // RF, chip ID register, always is ID_CH57*
#define R8_SAFE_ACCESS_ID                                                      \
    (*((uint8_ptr_t)0x40001042)) // RF, safe accessing ID register, always 0x04
#define R8_WDOG_COUNT                                                          \
    (*((uint8_ptr_t)0x40001043)) // RW, watch-dog count, count by clock
                                 // frequency Fsys/131072

#define R8_PLL_CONFIG                                                          \
    (*((uint8_ptr_t)0x4000104B)) // RWA, PLL configuration control, SAM

#define R8_HFCK_PWR_CTRL                                                       \
    (*((uint8_ptr_t)0x4000100A)) // RWA, high frequency clock module power
                                 // control, SAM
#define RB_CLK_XT32M_PON                                                       \
    0x04 // RWA, external 32MHz oscillator power control: 0=power down, 1-power
         // on
#define RB_CLK_PLL_PON                                                         \
    0x10 /* RWA, PLL power control: 0=power down, 1-power on                   \
          * Fck32k = RB_CLK_OSC32K_XT                                          \
          *     ? XT_32KHz                                                     \
          *     : RC_32KHz                                                     \
          * Fpll = XT_32MHz * 15 = 480MHz                                      \
          * Fsys = RB_CLK_SYS_MOD==3                                           \
          *     ? Fck32k                                                       \
          *     : ( ( RB_CLK_SYS_MOD[0] ? Fpll : XT_32MHz ) / RB_CLK_PLL_DIV ) \
          * default: Fsys = XT_32MHz / RB_CLK_PLL_DIV = 32MHz / 5 = 6.4MHz     \
          *   range: 32KHz, 2MHz~10MHz, 15MHz~60MHz                            \
          */

#define R8_FLASH_CFG                                                           \
    (*((uint8_ptr_t)0x40001807)) // RW, flash ROM access config, SAM


__attribute__((section(".highcode")))
static void sys_set_clock(void *clock)
{
    enum ch573_clock_source sc = *(enum ch573_clock_source *)clock;
    uint32_t        i;

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_PLL_CONFIG &= ~(1 << 5); //
    R8_SAFE_ACCESS_SIG = 0;
    if (sc & 0x20) { // HSE div
        if (!(R8_HFCK_PWR_CTRL & RB_CLK_XT32M_PON)) {
            R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
            R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
            R8_HFCK_PWR_CTRL |= RB_CLK_XT32M_PON; // HSE power on
            for (i = 0; i < 1200; i++) {
                __nop();
                __nop();
            }
        }

        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R16_CLK_SYS_CFG    = (0 << 6) | (sc & 0x1f);
        __nop();
        __nop();
        __nop();
        __nop();
        R8_SAFE_ACCESS_SIG = 0;
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R8_FLASH_CFG       = 0X01;
        R8_SAFE_ACCESS_SIG = 0;
    }

    else if (sc & 0x40) { // PLL div
        if (!(R8_HFCK_PWR_CTRL & RB_CLK_PLL_PON)) {
            R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
            R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
            R8_HFCK_PWR_CTRL |= RB_CLK_PLL_PON; // PLL power on
            for (i = 0; i < 2000; i++) {
                __nop();
                __nop();
            }
        }
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R16_CLK_SYS_CFG    = (1 << 6) | (sc & 0x1f);
        __nop();
        __nop();
        __nop();
        __nop();
        R8_SAFE_ACCESS_SIG = 0;
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R8_FLASH_CFG       = 0X03;
        R8_SAFE_ACCESS_SIG = 0;
    }
}
static uint32_t sys_get_clock(void *unit)
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

eer_sys_handler_t eer_hw_sys = {
    .clock = {
        .set = sys_set_clock,
        .get = sys_get_clock,
    },
};
