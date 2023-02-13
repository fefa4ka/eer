#pragma once

#include <hal.h>


#define __nop() asm volatile("nop")

typedef volatile uint8_t * uint8_ptr_t;
typedef volatile uint16_t *uint16_ptr_t;
typedef volatile uint32_t *uint32_ptr_t;

#define _r_              volatile const /* defines 'read only' permissions */
#define __w              volatile       /* defines 'write only' permissions */
#define _rw              volatile       /* defines 'read / write' permissions */
#define r__(index, size) uint8_t RESERVED##index[size]

#if defined(HAL_gpio) || defined(HAL_uart)
    #include "gpio.h"
extern eer_gpio_handler_t eer_hw_gpio;
#endif
#if defined(HAL_sys) || defined(HAL_timer) || defined(HAL_uart)
extern eer_sys_handler_t eer_hw_sys;
#endif
#ifdef HAL_timer
extern eer_timer_handler_t eer_hw_timer;
#endif
#ifdef HAL_uart
extern eer_serial_handler_t eer_hw_uart;
#endif
#ifdef HAL_ble
extern eer_ble_handler_t eer_hw_ble;
#endif

extern uint32_t SystemCoreClock;

/* Peripheral memory map */
#define FLASH_BASE                              ((uint32_t)0x08000000) /* FLASH base address in the alias region */
#define SRAM_BASE                               ((uint32_t)0x20000000) /* SRAM base address in the alias region */
#define PERIPH_BASE                             ((uint32_t)0x40000000) /* Peripheral base address in the alias region */

#define APB1PERIPH_BASE                         (PERIPH_BASE)
#define APB2PERIPH_BASE                         (PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE                          (PERIPH_BASE + 0x20000)


#define HSE_VALUE                 ((uint32_t)24000000) /* Value of the External oscillator in Hz */

/* In the following line adjust the External High Speed oscillator (HSE) Startup Timeout value */
#define HSE_STARTUP_TIMEOUT       ((uint16_t)0x2000) /* Time out for HSE start up */

#define HSI_VALUE                 ((uint32_t)24000000) /* Value of the Internal oscillator in Hz */


/* Interrupt Number Definition, according to the selected device */
typedef enum IRQn
{
    /******  RISC-V Processor Exceptions Numbers *******************************************************/
    NonMaskableInt_IRQn = 2, /* 2 Non Maskable Interrupt                             */
    EXC_IRQn = 3,            /* 3 Exception Interrupt                                */
    SysTick_IRQn = 12,       /* 12 System timer Interrupt                            */
    Software_IRQn = 14,      /* 14 software Interrupt                                */

    /******  RISC-V specific Interrupt Numbers *********************************************************/
    WWDG_IRQn = 16,          /* Window WatchDog Interrupt                            */
    PVD_IRQn = 17,           /* PVD through EXTI Line detection Interrupt            */
    FLASH_IRQn = 18,         /* FLASH global Interrupt                               */
    RCC_IRQn = 19,           /* RCC global Interrupt                                 */
    EXTI7_0_IRQn = 20,       /* External Line[7:0] Interrupts                        */
    AWU_IRQn = 21,           /* AWU global Interrupt                                 */
    DMA1_Channel1_IRQn = 22, /* DMA1 Channel 1 global Interrupt                      */
    DMA1_Channel2_IRQn = 23, /* DMA1 Channel 2 global Interrupt                      */
    DMA1_Channel3_IRQn = 24, /* DMA1 Channel 3 global Interrupt                      */
    DMA1_Channel4_IRQn = 25, /* DMA1 Channel 4 global Interrupt                      */
    DMA1_Channel5_IRQn = 26, /* DMA1 Channel 5 global Interrupt                      */
    DMA1_Channel6_IRQn = 27, /* DMA1 Channel 6 global Interrupt                      */
    DMA1_Channel7_IRQn = 28, /* DMA1 Channel 7 global Interrupt                      */
    ADC_IRQn = 29,           /* ADC global Interrupt                                 */
    I2C1_EV_IRQn = 30,       /* I2C1 Event Interrupt                                 */
    I2C1_ER_IRQn = 31,       /* I2C1 Error Interrupt                                 */
    USART1_IRQn = 32,        /* USART1 global Interrupt                              */
    SPI1_IRQn = 33,          /* SPI1 global Interrupt                                */
    TIM1_BRK_IRQn = 34,      /* TIM1 Break Interrupt                                 */
    TIM1_UP_IRQn = 35,       /* TIM1 Update Interrupt                                */
    TIM1_TRG_COM_IRQn = 36,  /* TIM1 Trigger and Commutation Interrupt               */
    TIM1_CC_IRQn = 37,       /* TIM1 Capture Compare Interrupt                       */
    TIM2_IRQn = 38,          /* TIM2 global Interrupt                                */

} IRQn_Type;

#define HardFault_IRQn    EXC_IRQn

/* memory mapped structure for Program Fast Interrupt Controller (PFIC) */
typedef struct{
    /* Interrupt Enable Status Register */
    _r_  uint32_t ISR[8];
    /* Interrupt Pending Status Register */
    _r_  uint32_t IPR[8];
    /* Interrupt Priority Threshold Configuration Register */
    _rw uint32_t ITHRESDR;
    _rw uint32_t RESERVED;
    /* Interrupt Configuration Register */
    _rw uint32_t CFGR;
    /* Interrupt Global Status Register */
    _r_  uint32_t GISR;
    _rw uint8_t VTFIDR[4];
    r__(0, 12);
    _rw uint32_t VTFADDR[4];
    r__(1, 0x90);
      /* Interrupt Enable Setting Register */
    __w  uint32_t IENR[8];
    r__(2, 0x60);
    /* Interrupt Enable Clear Register */
    __w  uint32_t IRER[8];
    r__(3, 0x60);
 /* Interrupt Pending Setup Register */
    __w  uint32_t IPSR[8];
    r__(4, 0x60);
 /* Interrupt Pending Clear Register */
    __w  uint32_t IPRR[8];
    r__(5, 0x60);
 /* Interrupt Active Status Register */
    _rw uint32_t IACTR[8];
    r__(6, 0xE0);
 /* Interrupt Priority Configuration Register */
    _rw uint8_t IPRIOR[256];
    r__(7, 0x810);
 /* System Control Register */
    _rw uint32_t SCTLR;
} PFIC_Type;

#define PFIC            ((PFIC_Type *) 0xE000E000 )
#define NVIC            PFIC
#define NVIC_KEY1       ((uint32_t)0xFA050000)
#define	NVIC_KEY2	    ((uint32_t)0xBCAF0000)
#define	NVIC_KEY3		((uint32_t)0xBEEF0000)

#define eer_hw_isr_enable() __asm volatile ("csrw mstatus, %0" : : "r" (0x1888) );
#define eer_hw_isr_disable() __asm volatile ("csrw mstatus, %0" : : "r" (0x1800) );

static inline void eer_hw_isr_enable_system(IRQn_Type IRQn) {
  NVIC->IENR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
  }

static inline void eer_hw_isr_disable_system(IRQn_Type IRQn) {
  NVIC->IRER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}


/* Reset and Clock Control */
typedef struct
{
    _rw uint32_t CTLR;
    _rw uint32_t CFGR0;
    _rw uint32_t INTR;
    _rw uint32_t APB2PRSTR;
    _rw uint32_t APB1PRSTR;
    _rw uint32_t AHBPCENR;
    _rw uint32_t APB2PCENR;
    _rw uint32_t APB1PCENR;
    _rw uint32_t RESERVED0;
    _rw uint32_t RSTSCKR;
} RCC_TypeDef;

#define RCC_BASE                                (AHBPERIPH_BASE + 0x1000)
#define RCC                                     ((RCC_TypeDef *)RCC_BASE)

/******************************************************************************/
/*                         Reset and Clock Control                            */
/******************************************************************************/

/********************  Bit definition for RCC_CTLR register  ********************/
#define RCC_HSION                               ((uint32_t)0x00000001) /* Internal High Speed clock enable */
#define RCC_HSIRDY                              ((uint32_t)0x00000002) /* Internal High Speed clock ready flag */
#define RCC_HSITRIM                             ((uint32_t)0x000000F8) /* Internal High Speed clock trimming */
#define RCC_HSICAL                              ((uint32_t)0x0000FF00) /* Internal High Speed clock Calibration */
#define RCC_HSEON                               ((uint32_t)0x00010000) /* External High Speed clock enable */
#define RCC_HSERDY                              ((uint32_t)0x00020000) /* External High Speed clock ready flag */
#define RCC_HSEBYP                              ((uint32_t)0x00040000) /* External High Speed clock Bypass */
#define RCC_CSSON                               ((uint32_t)0x00080000) /* Clock Security System enable */
#define RCC_PLLON                               ((uint32_t)0x01000000) /* PLL enable */
#define RCC_PLLRDY                              ((uint32_t)0x02000000) /* PLL clock ready flag */

/*******************  Bit definition for RCC_CFGR0 register  *******************/
#define RCC_SW                                  ((uint32_t)0x00000003) /* SW[1:0] bits (System clock Switch) */
#define RCC_SW_0                                ((uint32_t)0x00000001) /* Bit 0 */
#define RCC_SW_1                                ((uint32_t)0x00000002) /* Bit 1 */

#define RCC_SW_HSI                              ((uint32_t)0x00000000) /* HSI selected as system clock */
#define RCC_SW_HSE                              ((uint32_t)0x00000001) /* HSE selected as system clock */
#define RCC_SW_PLL                              ((uint32_t)0x00000002) /* PLL selected as system clock */

#define RCC_SWS                                 ((uint32_t)0x0000000C) /* SWS[1:0] bits (System Clock Switch Status) */
#define RCC_SWS_0                               ((uint32_t)0x00000004) /* Bit 0 */
#define RCC_SWS_1                               ((uint32_t)0x00000008) /* Bit 1 */

#define RCC_SWS_HSI                             ((uint32_t)0x00000000) /* HSI oscillator used as system clock */
#define RCC_SWS_HSE                             ((uint32_t)0x00000004) /* HSE oscillator used as system clock */
#define RCC_SWS_PLL                             ((uint32_t)0x00000008) /* PLL used as system clock */

#define RCC_HPRE                                ((uint32_t)0x000000F0) /* HPRE[3:0] bits (AHB prescaler) */
#define RCC_HPRE_0                              ((uint32_t)0x00000010) /* Bit 0 */
#define RCC_HPRE_1                              ((uint32_t)0x00000020) /* Bit 1 */
#define RCC_HPRE_2                              ((uint32_t)0x00000040) /* Bit 2 */
#define RCC_HPRE_3                              ((uint32_t)0x00000080) /* Bit 3 */

#define RCC_HPRE_DIV1                           ((uint32_t)0x00000000) /* SYSCLK not divided */
#define RCC_HPRE_DIV2                           ((uint32_t)0x00000010) /* SYSCLK divided by 2 */
#define RCC_HPRE_DIV3                           ((uint32_t)0x00000020) /* SYSCLK divided by 3 */
#define RCC_HPRE_DIV4                           ((uint32_t)0x00000030) /* SYSCLK divided by 4 */
#define RCC_HPRE_DIV5                           ((uint32_t)0x00000040) /* SYSCLK divided by 5 */
#define RCC_HPRE_DIV6                           ((uint32_t)0x00000050) /* SYSCLK divided by 6 */
#define RCC_HPRE_DIV7                           ((uint32_t)0x00000060) /* SYSCLK divided by 7 */
#define RCC_HPRE_DIV8                           ((uint32_t)0x00000070) /* SYSCLK divided by 8 */
#define RCC_HPRE_DIV16                          ((uint32_t)0x000000B0) /* SYSCLK divided by 16 */
#define RCC_HPRE_DIV32                          ((uint32_t)0x000000C0) /* SYSCLK divided by 32 */
#define RCC_HPRE_DIV64                          ((uint32_t)0x000000D0) /* SYSCLK divided by 64 */
#define RCC_HPRE_DIV128                         ((uint32_t)0x000000E0) /* SYSCLK divided by 128 */
#define RCC_HPRE_DIV256                         ((uint32_t)0x000000F0) /* SYSCLK divided by 256 */

#define RCC_PPRE1                               ((uint32_t)0x00000700) /* PRE1[2:0] bits (APB1 prescaler) */
#define RCC_PPRE1_0                             ((uint32_t)0x00000100) /* Bit 0 */
#define RCC_PPRE1_1                             ((uint32_t)0x00000200) /* Bit 1 */
#define RCC_PPRE1_2                             ((uint32_t)0x00000400) /* Bit 2 */

#define RCC_PPRE1_DIV1                          ((uint32_t)0x00000000) /* HCLK not divided */
#define RCC_PPRE1_DIV2                          ((uint32_t)0x00000400) /* HCLK divided by 2 */
#define RCC_PPRE1_DIV4                          ((uint32_t)0x00000500) /* HCLK divided by 4 */
#define RCC_PPRE1_DIV8                          ((uint32_t)0x00000600) /* HCLK divided by 8 */
#define RCC_PPRE1_DIV16                         ((uint32_t)0x00000700) /* HCLK divided by 16 */

#define RCC_PPRE2                               ((uint32_t)0x00003800) /* PRE2[2:0] bits (APB2 prescaler) */
#define RCC_PPRE2_0                             ((uint32_t)0x00000800) /* Bit 0 */
#define RCC_PPRE2_1                             ((uint32_t)0x00001000) /* Bit 1 */
#define RCC_PPRE2_2                             ((uint32_t)0x00002000) /* Bit 2 */

#define RCC_PPRE2_DIV1                          ((uint32_t)0x00000000) /* HCLK not divided */
#define RCC_PPRE2_DIV2                          ((uint32_t)0x00002000) /* HCLK divided by 2 */
#define RCC_PPRE2_DIV4                          ((uint32_t)0x00002800) /* HCLK divided by 4 */
#define RCC_PPRE2_DIV8                          ((uint32_t)0x00003000) /* HCLK divided by 8 */
#define RCC_PPRE2_DIV16                         ((uint32_t)0x00003800) /* HCLK divided by 16 */

#define RCC_ADCPRE                              ((uint32_t)0x0000C000) /* ADCPRE[1:0] bits (ADC prescaler) */
#define RCC_ADCPRE_0                            ((uint32_t)0x00004000) /* Bit 0 */
#define RCC_ADCPRE_1                            ((uint32_t)0x00008000) /* Bit 1 */

#define RCC_ADCPRE_DIV2                         ((uint32_t)0x00000000) /* PCLK2 divided by 2 */
#define RCC_ADCPRE_DIV4                         ((uint32_t)0x00004000) /* PCLK2 divided by 4 */
#define RCC_ADCPRE_DIV6                         ((uint32_t)0x00008000) /* PCLK2 divided by 6 */
#define RCC_ADCPRE_DIV8                         ((uint32_t)0x0000C000) /* PCLK2 divided by 8 */

#define RCC_PLLSRC                              ((uint32_t)0x00010000) /* PLL entry clock source */

#define RCC_PLLXTPRE                            ((uint32_t)0x00020000) /* HSE divider for PLL entry */

#define RCC_PLLMULL                             ((uint32_t)0x003C0000) /* PLLMUL[3:0] bits (PLL multiplication factor) */
#define RCC_PLLMULL_0                           ((uint32_t)0x00040000) /* Bit 0 */
#define RCC_PLLMULL_1                           ((uint32_t)0x00080000) /* Bit 1 */
#define RCC_PLLMULL_2                           ((uint32_t)0x00100000) /* Bit 2 */
#define RCC_PLLMULL_3                           ((uint32_t)0x00200000) /* Bit 3 */

#define RCC_PLLSRC_HSI_Mul2                     ((uint32_t)0x00000000) /* HSI clock*2 selected as PLL entry clock source */
#define RCC_PLLSRC_HSE_Mul2                     ((uint32_t)0x00010000) /* HSE clock*2 selected as PLL entry clock source */

#define RCC_PLLXTPRE_HSE                        ((uint32_t)0x00000000) /* HSE clock not divided for PLL entry */
#define RCC_PLLXTPRE_HSE_Div2                   ((uint32_t)0x00020000) /* HSE clock divided by 2 for PLL entry */

#define RCC_PLLMULL2                            ((uint32_t)0x00000000) /* PLL input clock*2 */
#define RCC_PLLMULL3                            ((uint32_t)0x00040000) /* PLL input clock*3 */
#define RCC_PLLMULL4                            ((uint32_t)0x00080000) /* PLL input clock*4 */
#define RCC_PLLMULL5                            ((uint32_t)0x000C0000) /* PLL input clock*5 */
#define RCC_PLLMULL6                            ((uint32_t)0x00100000) /* PLL input clock*6 */
#define RCC_PLLMULL7                            ((uint32_t)0x00140000) /* PLL input clock*7 */
#define RCC_PLLMULL8                            ((uint32_t)0x00180000) /* PLL input clock*8 */
#define RCC_PLLMULL9                            ((uint32_t)0x001C0000) /* PLL input clock*9 */
#define RCC_PLLMULL10                           ((uint32_t)0x00200000) /* PLL input clock10 */
#define RCC_PLLMULL11                           ((uint32_t)0x00240000) /* PLL input clock*11 */
#define RCC_PLLMULL12                           ((uint32_t)0x00280000) /* PLL input clock*12 */
#define RCC_PLLMULL13                           ((uint32_t)0x002C0000) /* PLL input clock*13 */
#define RCC_PLLMULL14                           ((uint32_t)0x00300000) /* PLL input clock*14 */
#define RCC_PLLMULL15                           ((uint32_t)0x00340000) /* PLL input clock*15 */
#define RCC_PLLMULL16                           ((uint32_t)0x00380000) /* PLL input clock*16 */
#define RCC_USBPRE                              ((uint32_t)0x00400000) /* USB Device prescaler */

#define RCC_CFGR0_MCO                           ((uint32_t)0x07000000) /* MCO[2:0] bits (Microcontroller Clock Output) */
#define RCC_MCO_0                               ((uint32_t)0x01000000) /* Bit 0 */
#define RCC_MCO_1                               ((uint32_t)0x02000000) /* Bit 1 */
#define RCC_MCO_2                               ((uint32_t)0x04000000) /* Bit 2 */

#define RCC_MCO_NOCLOCK                         ((uint32_t)0x00000000) /* No clock */
#define RCC_CFGR0_MCO_SYSCLK                    ((uint32_t)0x04000000) /* System clock selected as MCO source */
#define RCC_CFGR0_MCO_HSI                       ((uint32_t)0x05000000) /* HSI clock selected as MCO source */
#define RCC_CFGR0_MCO_HSE                       ((uint32_t)0x06000000) /* HSE clock selected as MCO source  */
#define RCC_CFGR0_MCO_PLL                       ((uint32_t)0x07000000) /* PLL clock divided by 2 selected as MCO source */

/*******************  Bit definition for RCC_INTR register  ********************/
#define RCC_LSIRDYF                             ((uint32_t)0x00000001) /* LSI Ready Interrupt flag */
#define RCC_LSERDYF                             ((uint32_t)0x00000002) /* LSE Ready Interrupt flag */
#define RCC_HSIRDYF                             ((uint32_t)0x00000004) /* HSI Ready Interrupt flag */
#define RCC_HSERDYF                             ((uint32_t)0x00000008) /* HSE Ready Interrupt flag */
#define RCC_PLLRDYF                             ((uint32_t)0x00000010) /* PLL Ready Interrupt flag */
#define RCC_CSSF                                ((uint32_t)0x00000080) /* Clock Security System Interrupt flag */
#define RCC_LSIRDYIE                            ((uint32_t)0x00000100) /* LSI Ready Interrupt Enable */
#define RCC_LSERDYIE                            ((uint32_t)0x00000200) /* LSE Ready Interrupt Enable */
#define RCC_HSIRDYIE                            ((uint32_t)0x00000400) /* HSI Ready Interrupt Enable */
#define RCC_HSERDYIE                            ((uint32_t)0x00000800) /* HSE Ready Interrupt Enable */
#define RCC_PLLRDYIE                            ((uint32_t)0x00001000) /* PLL Ready Interrupt Enable */
#define RCC_LSIRDYC                             ((uint32_t)0x00010000) /* LSI Ready Interrupt Clear */
#define RCC_LSERDYC                             ((uint32_t)0x00020000) /* LSE Ready Interrupt Clear */
#define RCC_HSIRDYC                             ((uint32_t)0x00040000) /* HSI Ready Interrupt Clear */
#define RCC_HSERDYC                             ((uint32_t)0x00080000) /* HSE Ready Interrupt Clear */
#define RCC_PLLRDYC                             ((uint32_t)0x00100000) /* PLL Ready Interrupt Clear */
#define RCC_CSSC                                ((uint32_t)0x00800000) /* Clock Security System Interrupt Clear */

/*****************  Bit definition for RCC_APB2PRSTR register  *****************/
#define RCC_AFIORST                             ((uint32_t)0x00000001) /* Alternate Function I/O reset */
#define RCC_IOPARST                             ((uint32_t)0x00000004) /* I/O port A reset */
#define RCC_IOPBRST                             ((uint32_t)0x00000008) /* I/O port B reset */
#define RCC_IOPCRST                             ((uint32_t)0x00000010) /* I/O port C reset */
#define RCC_IOPDRST                             ((uint32_t)0x00000020) /* I/O port D reset */
#define RCC_ADC1RST                             ((uint32_t)0x00000200) /* ADC 1 interface reset */

#define RCC_ADC2RST                             ((uint32_t)0x00000400) /* ADC 2 interface reset */

#define RCC_TIM1RST                             ((uint32_t)0x00000800) /* TIM1 Timer reset */
#define RCC_SPI1RST                             ((uint32_t)0x00001000) /* SPI 1 reset */
#define RCC_USART1RST                           ((uint32_t)0x00004000) /* USART1 reset */

#define RCC_IOPERST                             ((uint32_t)0x00000040) /* I/O port E reset */

/*****************  Bit definition for RCC_APB1PRSTR register  *****************/
#define RCC_TIM2RST                             ((uint32_t)0x00000001) /* Timer 2 reset */
#define RCC_TIM3RST                             ((uint32_t)0x00000002) /* Timer 3 reset */
#define RCC_WWDGRST                             ((uint32_t)0x00000800) /* Window Watchdog reset */
#define RCC_USART2RST                           ((uint32_t)0x00020000) /* USART 2 reset */
#define RCC_I2C1RST                             ((uint32_t)0x00200000) /* I2C 1 reset */

#define RCC_CAN1RST                             ((uint32_t)0x02000000) /* CAN1 reset */

#define RCC_BKPRST                              ((uint32_t)0x08000000) /* Backup interface reset */
#define RCC_PWRRST                              ((uint32_t)0x10000000) /* Power interface reset */

#define RCC_TIM4RST                             ((uint32_t)0x00000004) /* Timer 4 reset */
#define RCC_SPI2RST                             ((uint32_t)0x00004000) /* SPI 2 reset */
#define RCC_USART3RST                           ((uint32_t)0x00040000) /* USART 3 reset */
#define RCC_I2C2RST                             ((uint32_t)0x00400000) /* I2C 2 reset */

#define RCC_USBRST                              ((uint32_t)0x00800000) /* USB Device reset */

/******************  Bit definition for RCC_AHBPCENR register  ******************/
#define RCC_DMA1EN                              ((uint16_t)0x0001) /* DMA1 clock enable */
#define RCC_SRAMEN                              ((uint16_t)0x0004) /* SRAM interface clock enable */
#define RCC_FLITFEN                             ((uint16_t)0x0010) /* FLITF clock enable */
#define RCC_CRCEN                               ((uint16_t)0x0040) /* CRC clock enable */
#define RCC_USBHD                               ((uint16_t)0x1000)

/******************  Bit definition for RCC_APB2PCENR register  *****************/
#define RCC_AFIOEN                              ((uint32_t)0x00000001) /* Alternate Function I/O clock enable */
#define RCC_IOPAEN                              ((uint32_t)0x00000004) /* I/O port A clock enable */
#define RCC_IOPBEN                              ((uint32_t)0x00000008) /* I/O port B clock enable */
#define RCC_IOPCEN                              ((uint32_t)0x00000010) /* I/O port C clock enable */
#define RCC_IOPDEN                              ((uint32_t)0x00000020) /* I/O port D clock enable */
#define RCC_ADC1EN                              ((uint32_t)0x00000200) /* ADC 1 interface clock enable */

#define RCC_ADC2EN                              ((uint32_t)0x00000400) /* ADC 2 interface clock enable */

#define RCC_TIM1EN                              ((uint32_t)0x00000800) /* TIM1 Timer clock enable */
#define RCC_SPI1EN                              ((uint32_t)0x00001000) /* SPI 1 clock enable */
#define RCC_USART1EN                            ((uint32_t)0x00004000) /* USART1 clock enable */

/*****************  Bit definition for RCC_APB1PCENR register  ******************/
#define RCC_TIM2EN                              ((uint32_t)0x00000001) /* Timer 2 clock enabled*/
#define RCC_TIM3EN                              ((uint32_t)0x00000002) /* Timer 3 clock enable */
#define RCC_WWDGEN                              ((uint32_t)0x00000800) /* Window Watchdog clock enable */
#define RCC_USART2EN                            ((uint32_t)0x00020000) /* USART 2 clock enable */
#define RCC_I2C1EN                              ((uint32_t)0x00200000) /* I2C 1 clock enable */

#define RCC_BKPEN                               ((uint32_t)0x08000000) /* Backup interface clock enable */
#define RCC_PWREN                               ((uint32_t)0x10000000) /* Power interface clock enable */

#define RCC_USBEN                               ((uint32_t)0x00800000) /* USB Device clock enable */

/*******************  Bit definition for RCC_RSTSCKR register  ********************/
#define RCC_LSION                               ((uint32_t)0x00000001) /* Internal Low Speed oscillator enable */
#define RCC_LSIRDY                              ((uint32_t)0x00000002) /* Internal Low Speed oscillator Ready */
#define RCC_RMVF                                ((uint32_t)0x01000000) /* Remove reset flag */
#define RCC_PINRSTF                             ((uint32_t)0x04000000) /* PIN reset flag */
#define RCC_PORRSTF                             ((uint32_t)0x08000000) /* POR/PDR reset flag */
#define RCC_SFTRSTF                             ((uint32_t)0x10000000) /* Software Reset flag */
#define RCC_IWDGRSTF                            ((uint32_t)0x20000000) /* Independent Watchdog reset flag */
#define RCC_WWDGRSTF                            ((uint32_t)0x40000000) /* Window watchdog reset flag */
#define RCC_LPWRRSTF                            ((uint32_t)0x80000000) /* Low-Power reset flag */

typedef uint32_t timer_size_t;
#define TIMER_MAX UINT32_MAX


/* Alternate Function I/O */
typedef struct
{
    uint32_t RESERVED0;
    _rw uint32_t PCFR1;
    _rw uint32_t EXTICR;
} AFIO_TypeDef;
#define AFIO_BASE                               (APB2PERIPH_BASE + 0x0000)
#define AFIO                                    ((AFIO_TypeDef *)AFIO_BASE)

/******************  Bit definition for AFIO_PCFR1register  *******************/
#define AFIO_PCFR1_SPI1_REMAP                   ((uint32_t)0x00000001) /* SPI1 remapping */
#define AFIO_PCFR1_I2C1_REMAP                   ((uint32_t)0x00000002) /* I2C1 remapping */
#define AFIO_PCFR1_USART1_REMAP                 ((uint32_t)0x00000004) /* USART1 remapping */
#define AFIO_PCFR1_USART2_REMAP                 ((uint32_t)0x00000008) /* USART2 remapping */

#define AFIO_PCFR1_USART3_REMAP                 ((uint32_t)0x00000030) /* USART3_REMAP[1:0] bits (USART3 remapping) */
#define AFIO_PCFR1_USART3_REMAP_0               ((uint32_t)0x00000010) /* Bit 0 */
#define AFIO_PCFR1_USART3_REMAP_1               ((uint32_t)0x00000020) /* Bit 1 */

#define AFIO_PCFR1_USART3_REMAP_NOREMAP         ((uint32_t)0x00000000) /* No remap (TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14) */
#define AFIO_PCFR1_USART3_REMAP_PARTIALREMAP    ((uint32_t)0x00000010) /* Partial remap (TX/PC10, RX/PC11, CK/PC12, CTS/PB13, RTS/PB14) */
#define AFIO_PCFR1_USART3_REMAP_FULLREMAP       ((uint32_t)0x00000030) /* Full remap (TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12) */

#define AFIO_PCFR1_TIM1_REMAP                   ((uint32_t)0x000000C0) /* TIM1_REMAP[1:0] bits (TIM1 remapping) */
#define AFIO_PCFR1_TIM1_REMAP_0                 ((uint32_t)0x00000040) /* Bit 0 */
#define AFIO_PCFR1_TIM1_REMAP_1                 ((uint32_t)0x00000080) /* Bit 1 */

#define AFIO_PCFR1_TIM1_REMAP_NOREMAP           ((uint32_t)0x00000000) /* No remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PB12, CH1N/PB13, CH2N/PB14, CH3N/PB15) */
#define AFIO_PCFR1_TIM1_REMAP_PARTIALREMAP      ((uint32_t)0x00000040) /* Partial remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PA6, CH1N/PA7, CH2N/PB0, CH3N/PB1) */
#define AFIO_PCFR1_TIM1_REMAP_FULLREMAP         ((uint32_t)0x000000C0) /* Full remap (ETR/PE7, CH1/PE9, CH2/PE11, CH3/PE13, CH4/PE14, BKIN/PE15, CH1N/PE8, CH2N/PE10, CH3N/PE12) */

#define AFIO_PCFR1_TIM2_REMAP                   ((uint32_t)0x00000300) /* TIM2_REMAP[1:0] bits (TIM2 remapping) */
#define AFIO_PCFR1_TIM2_REMAP_0                 ((uint32_t)0x00000100) /* Bit 0 */
#define AFIO_PCFR1_TIM2_REMAP_1                 ((uint32_t)0x00000200) /* Bit 1 */

#define AFIO_PCFR1_TIM2_REMAP_NOREMAP           ((uint32_t)0x00000000) /* No remap (CH1/ETR/PA0, CH2/PA1, CH3/PA2, CH4/PA3) */
#define AFIO_PCFR1_TIM2_REMAP_PARTIALREMAP1     ((uint32_t)0x00000100) /* Partial remap (CH1/ETR/PA15, CH2/PB3, CH3/PA2, CH4/PA3) */
#define AFIO_PCFR1_TIM2_REMAP_PARTIALREMAP2     ((uint32_t)0x00000200) /* Partial remap (CH1/ETR/PA0, CH2/PA1, CH3/PB10, CH4/PB11) */
#define AFIO_PCFR1_TIM2_REMAP_FULLREMAP         ((uint32_t)0x00000300) /* Full remap (CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11) */

#define AFIO_PCFR1_TIM3_REMAP                   ((uint32_t)0x00000C00) /* TIM3_REMAP[1:0] bits (TIM3 remapping) */
#define AFIO_PCFR1_TIM3_REMAP_0                 ((uint32_t)0x00000400) /* Bit 0 */
#define AFIO_PCFR1_TIM3_REMAP_1                 ((uint32_t)0x00000800) /* Bit 1 */

#define AFIO_PCFR1_TIM3_REMAP_NOREMAP           ((uint32_t)0x00000000) /* No remap (CH1/PA6, CH2/PA7, CH3/PB0, CH4/PB1) */
#define AFIO_PCFR1_TIM3_REMAP_PARTIALREMAP      ((uint32_t)0x00000800) /* Partial remap (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1) */
#define AFIO_PCFR1_TIM3_REMAP_FULLREMAP         ((uint32_t)0x00000C00) /* Full remap (CH1/PC6, CH2/PC7, CH3/PC8, CH4/PC9) */

#define AFIO_PCFR1_TIM4_REMAP                   ((uint32_t)0x00001000) /* TIM4_REMAP bit (TIM4 remapping) */

#define AFIO_PCFR1_CAN_REMAP                    ((uint32_t)0x00006000) /* CAN_REMAP[1:0] bits (CAN Alternate function remapping) */
#define AFIO_PCFR1_CAN_REMAP_0                  ((uint32_t)0x00002000) /* Bit 0 */
#define AFIO_PCFR1_CAN_REMAP_1                  ((uint32_t)0x00004000) /* Bit 1 */

#define AFIO_PCFR1_CAN_REMAP_REMAP1             ((uint32_t)0x00000000) /* CANRX mapped to PA11, CANTX mapped to PA12 */
#define AFIO_PCFR1_CAN_REMAP_REMAP2             ((uint32_t)0x00004000) /* CANRX mapped to PB8, CANTX mapped to PB9 */
#define AFIO_PCFR1_CAN_REMAP_REMAP3             ((uint32_t)0x00006000) /* CANRX mapped to PD0, CANTX mapped to PD1 */

#define AFIO_PCFR1_PA12_REMAP                   ((uint32_t)0x00008000) /* Port D0/Port D1 mapping on OSC_IN/OSC_OUT */
#define AFIO_PCFR1_TIM5CH4_IREMAP               ((uint32_t)0x00010000) /* TIM5 Channel4 Internal Remap */
#define AFIO_PCFR1_ADC1_ETRGINJ_REMAP           ((uint32_t)0x00020000) /* ADC 1 External Trigger Injected Conversion remapping */
#define AFIO_PCFR1_ADC1_ETRGREG_REMAP           ((uint32_t)0x00040000) /* ADC 1 External Trigger Regular Conversion remapping */
#define AFIO_PCFR1_ADC2_ETRGINJ_REMAP           ((uint32_t)0x00080000) /* ADC 2 External Trigger Injected Conversion remapping */
#define AFIO_PCFR1_ADC2_ETRGREG_REMAP           ((uint32_t)0x00100000) /* ADC 2 External Trigger Regular Conversion remapping */

#define AFIO_PCFR1_SWJ_CFG                      ((uint32_t)0x07000000) /* SWJ_CFG[2:0] bits (Serial Wire JTAG configuration) */
#define AFIO_PCFR1_SWJ_CFG_0                    ((uint32_t)0x01000000) /* Bit 0 */
#define AFIO_PCFR1_SWJ_CFG_1                    ((uint32_t)0x02000000) /* Bit 1 */
#define AFIO_PCFR1_SWJ_CFG_2                    ((uint32_t)0x04000000) /* Bit 2 */

#define AFIO_PCFR1_SWJ_CFG_RESET                ((uint32_t)0x00000000) /* Full SWJ (JTAG-DP + SW-DP) : Reset State */
#define AFIO_PCFR1_SWJ_CFG_NOJNTRST             ((uint32_t)0x01000000) /* Full SWJ (JTAG-DP + SW-DP) but without JNTRST */
#define AFIO_PCFR1_SWJ_CFG_JTAGDISABLE          ((uint32_t)0x02000000) /* JTAG-DP Disabled and SW-DP Enabled */
#define AFIO_PCFR1_SWJ_CFG_DISABLE              ((uint32_t)0x04000000) /* JTAG-DP Disabled and SW-DP Disabled */

/*****************  Bit definition for AFIO_EXTICR1 register  *****************/
#define AFIO_EXTICR1_EXTI0                      ((uint16_t)0x000F) /* EXTI 0 configuration */
#define AFIO_EXTICR1_EXTI1                      ((uint16_t)0x00F0) /* EXTI 1 configuration */
#define AFIO_EXTICR1_EXTI2                      ((uint16_t)0x0F00) /* EXTI 2 configuration */
#define AFIO_EXTICR1_EXTI3                      ((uint16_t)0xF000) /* EXTI 3 configuration */

#define AFIO_EXTICR1_EXTI0_PA                   ((uint16_t)0x0000) /* PA[0] pin */
#define AFIO_EXTICR1_EXTI0_PB                   ((uint16_t)0x0001) /* PB[0] pin */
#define AFIO_EXTICR1_EXTI0_PC                   ((uint16_t)0x0002) /* PC[0] pin */
#define AFIO_EXTICR1_EXTI0_PD                   ((uint16_t)0x0003) /* PD[0] pin */
#define AFIO_EXTICR1_EXTI0_PE                   ((uint16_t)0x0004) /* PE[0] pin */
#define AFIO_EXTICR1_EXTI0_PF                   ((uint16_t)0x0005) /* PF[0] pin */
#define AFIO_EXTICR1_EXTI0_PG                   ((uint16_t)0x0006) /* PG[0] pin */

#define AFIO_EXTICR1_EXTI1_PA                   ((uint16_t)0x0000) /* PA[1] pin */
#define AFIO_EXTICR1_EXTI1_PB                   ((uint16_t)0x0010) /* PB[1] pin */
#define AFIO_EXTICR1_EXTI1_PC                   ((uint16_t)0x0020) /* PC[1] pin */
#define AFIO_EXTICR1_EXTI1_PD                   ((uint16_t)0x0030) /* PD[1] pin */
#define AFIO_EXTICR1_EXTI1_PE                   ((uint16_t)0x0040) /* PE[1] pin */
#define AFIO_EXTICR1_EXTI1_PF                   ((uint16_t)0x0050) /* PF[1] pin */
#define AFIO_EXTICR1_EXTI1_PG                   ((uint16_t)0x0060) /* PG[1] pin */

#define AFIO_EXTICR1_EXTI2_PA                   ((uint16_t)0x0000) /* PA[2] pin */
#define AFIO_EXTICR1_EXTI2_PB                   ((uint16_t)0x0100) /* PB[2] pin */
#define AFIO_EXTICR1_EXTI2_PC                   ((uint16_t)0x0200) /* PC[2] pin */
#define AFIO_EXTICR1_EXTI2_PD                   ((uint16_t)0x0300) /* PD[2] pin */
#define AFIO_EXTICR1_EXTI2_PE                   ((uint16_t)0x0400) /* PE[2] pin */
#define AFIO_EXTICR1_EXTI2_PF                   ((uint16_t)0x0500) /* PF[2] pin */
#define AFIO_EXTICR1_EXTI2_PG                   ((uint16_t)0x0600) /* PG[2] pin */

#define AFIO_EXTICR1_EXTI3_PA                   ((uint16_t)0x0000) /* PA[3] pin */
#define AFIO_EXTICR1_EXTI3_PB                   ((uint16_t)0x1000) /* PB[3] pin */
#define AFIO_EXTICR1_EXTI3_PC                   ((uint16_t)0x2000) /* PC[3] pin */
#define AFIO_EXTICR1_EXTI3_PD                   ((uint16_t)0x3000) /* PD[3] pin */
#define AFIO_EXTICR1_EXTI3_PE                   ((uint16_t)0x4000) /* PE[3] pin */
#define AFIO_EXTICR1_EXTI3_PF                   ((uint16_t)0x5000) /* PF[3] pin */
#define AFIO_EXTICR1_EXTI3_PG                   ((uint16_t)0x6000) /* PG[3] pin */


/* FLASH Registers */
typedef struct
{
    _rw uint32_t ACTLR;
    _rw uint32_t KEYR;
    _rw uint32_t OBKEYR;
    _rw uint32_t STATR;
    _rw uint32_t CTLR;
    _rw uint32_t ADDR;
    _rw uint32_t RESERVED;
    _rw uint32_t OBR;
    _rw uint32_t WPR;
    _rw uint32_t MODEKEYR;
    _rw uint32_t BOOT_MODEKEYR;
} FLASH_TypeDef;

#define FLASH_R_BASE                            (AHBPERIPH_BASE + 0x2000) /* Flash registers base address */
#define FLASH                                   ((FLASH_TypeDef *)FLASH_R_BASE)

/******************************************************************************/
/*                      FLASH and Option Bytes Registers                      */
/******************************************************************************/

/*******************  Bit definition for FLASH_ACTLR register  ******************/
#define FLASH_ACTLR_LATENCY                     ((uint8_t)0x03) /* LATENCY[2:0] bits (Latency) */
#define FLASH_ACTLR_LATENCY_0                   ((uint8_t)0x00) /* Bit 0 */
#define FLASH_ACTLR_LATENCY_1                   ((uint8_t)0x01) /* Bit 0 */
#define FLASH_ACTLR_LATENCY_2                   ((uint8_t)0x02) /* Bit 1 */

/******************  Bit definition for FLASH_KEYR register  ******************/
#define FLASH_KEYR_FKEYR                        ((uint32_t)0xFFFFFFFF) /* FPEC Key */

/*****************  Bit definition for FLASH_OBKEYR register  ****************/
#define FLASH_OBKEYR_OBKEYR                     ((uint32_t)0xFFFFFFFF) /* Option Byte Key */

/******************  Bit definition for FLASH_STATR register  *******************/
#define FLASH_STATR_BSY                         ((uint8_t)0x01) /* Busy */
#define FLASH_STATR_WRPRTERR                    ((uint8_t)0x10) /* Write Protection Error */
#define FLASH_STATR_EOP                         ((uint8_t)0x20) /* End of operation */

/*******************  Bit definition for FLASH_CTLR register  *******************/
#define FLASH_CTLR_PG                           ((uint16_t)0x0001)     /* Programming */
#define FLASH_CTLR_PER                          ((uint16_t)0x0002)     /* Page Erase 1KByte*/
#define FLASH_CTLR_MER                          ((uint16_t)0x0004)     /* Mass Erase */
#define FLASH_CTLR_OPTPG                        ((uint16_t)0x0010)     /* Option Byte Programming */
#define FLASH_CTLR_OPTER                        ((uint16_t)0x0020)     /* Option Byte Erase */
#define FLASH_CTLR_STRT                         ((uint16_t)0x0040)     /* Start */
#define FLASH_CTLR_LOCK                         ((uint16_t)0x0080)     /* Lock */
#define FLASH_CTLR_OPTWRE                       ((uint16_t)0x0200)     /* Option Bytes Write Enable */
#define FLASH_CTLR_ERRIE                        ((uint16_t)0x0400)     /* Error Interrupt Enable */
#define FLASH_CTLR_EOPIE                        ((uint16_t)0x1000)     /* End of operation interrupt enable */
#define FLASH_CTLR_PAGE_PG                      ((uint16_t)0x00010000) /* Page Programming 64Byte */
#define FLASH_CTLR_PAGE_ER                      ((uint16_t)0x00020000) /* Page Erase 64Byte */
#define FLASH_CTLR_BUF_LOAD                     ((uint16_t)0x00040000) /* Buffer Load */
#define FLASH_CTLR_BUF_RST                      ((uint16_t)0x00080000) /* Buffer Reset */

/*******************  Bit definition for FLASH_ADDR register  *******************/
#define FLASH_ADDR_FAR                          ((uint32_t)0xFFFFFFFF) /* Flash Address */

/******************  Bit definition for FLASH_OBR register  *******************/
#define FLASH_OBR_OPTERR                        ((uint16_t)0x0001) /* Option Byte Error */
#define FLASH_OBR_RDPRT                         ((uint16_t)0x0002) /* Read protection */

#define FLASH_OBR_USER                          ((uint16_t)0x03FC) /* User Option Bytes */
#define FLASH_OBR_WDG_SW                        ((uint16_t)0x0004) /* WDG_SW */
#define FLASH_OBR_nRST_STOP                     ((uint16_t)0x0008) /* nRST_STOP */
#define FLASH_OBR_nRST_STDBY                    ((uint16_t)0x0010) /* nRST_STDBY */
#define FLASH_OBR_RST_MODE                      ((uint16_t)0x0060) /* RST_MODE */

/******************  Bit definition for FLASH_WPR register  ******************/
#define FLASH_WPR_WRP                           ((uint32_t)0xFFFFFFFF) /* Write Protect */

/******************  Bit definition for FLASH_RDPR register  *******************/
#define FLASH_RDPR_RDPR                         ((uint32_t)0x000000FF) /* Read protection option byte */
#define FLASH_RDPR_nRDPR                        ((uint32_t)0x0000FF00) /* Read protection complemented option byte */

/******************  Bit definition for FLASH_USER register  ******************/
#define FLASH_USER_USER                         ((uint32_t)0x00FF0000) /* User option byte */
#define FLASH_USER_nUSER                        ((uint32_t)0xFF000000) /* User complemented option byte */

/******************  Bit definition for FLASH_Data0 register  *****************/
#define FLASH_Data0_Data0                       ((uint32_t)0x000000FF) /* User data storage option byte */
#define FLASH_Data0_nData0                      ((uint32_t)0x0000FF00) /* User data storage complemented option byte */

/******************  Bit definition for FLASH_Data1 register  *****************/
#define FLASH_Data1_Data1                       ((uint32_t)0x00FF0000) /* User data storage option byte */
#define FLASH_Data1_nData1                      ((uint32_t)0xFF000000) /* User data storage complemented option byte */

/******************  Bit definition for FLASH_WRPR0 register  ******************/
#define FLASH_WRPR0_WRPR0                       ((uint32_t)0x000000FF) /* Flash memory write protection option bytes */
#define FLASH_WRPR0_nWRPR0                      ((uint32_t)0x0000FF00) /* Flash memory write protection complemented option bytes */

/******************  Bit definition for FLASH_WRPR1 register  ******************/
#define FLASH_WRPR1_WRPR1                       ((uint32_t)0x00FF0000) /* Flash memory write protection option bytes */
#define FLASH_WRPR1_nWRPR1                      ((uint32_t)0xFF000000) /* Flash memory write protection complemented option bytes */


/* AHB_peripheral */
#define RCC_AHBPeriph_DMA1               ((uint32_t)0x00000001)
#define RCC_AHBPeriph_SRAM               ((uint32_t)0x00000004)

/* APB2_peripheral */
#define RCC_APB2Periph_AFIO              ((uint32_t)0x00000001)
#define RCC_APB2Periph_GPIOA             ((uint32_t)0x00000004)
#define RCC_APB2Periph_GPIOC             ((uint32_t)0x00000010)
#define RCC_APB2Periph_GPIOD             ((uint32_t)0x00000020)
#define RCC_APB2Periph_ADC1              ((uint32_t)0x00000200)
#define RCC_APB2Periph_TIM1              ((uint32_t)0x00000800)
#define RCC_APB2Periph_SPI1              ((uint32_t)0x00001000)
#define RCC_APB2Periph_USART1            ((uint32_t)0x00004000)

/* APB1_peripheral */
#define RCC_APB1Periph_TIM2              ((uint32_t)0x00000001)
#define RCC_APB1Periph_WWDG              ((uint32_t)0x00000800)
#define RCC_APB1Periph_I2C1              ((uint32_t)0x00200000)
#define RCC_APB1Periph_PWR               ((uint32_t)0x10000000)


/* Preemption_Priority_Group */
#define NVIC_PriorityGroup_0           ((uint32_t)0x00)
#define NVIC_PriorityGroup_1           ((uint32_t)0x01)
#define NVIC_PriorityGroup_2           ((uint32_t)0x02)
#define NVIC_PriorityGroup_3           ((uint32_t)0x03)
#define NVIC_PriorityGroup_4           ((uint32_t)0x04)
