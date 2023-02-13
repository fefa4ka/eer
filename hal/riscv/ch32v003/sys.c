#include "sys.h"

_r_ uint8_t AHBPrescTable[16]
    = {1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8};
_r_ uint8_t ADCPrescTable[20] = {2,  4,  6,  8,  4,  8,  12, 16, 8,  16,
                                 24, 32, 16, 32, 48, 64, 32, 64, 96, 128};

uint32_t SystemCoreClock;

/* System clock (SYSCLK) status (hardware set).
 *
 * 00: the system clock source is HSI.
 * 01: The system clock source is HSE.
 * 10: The system clock source is a PLL.
 * 11: Not available.
 */
#define CFGR0_SWS_Mask ((uint32_t)0x0000000C)

/* Input clock source for PLL (write only when PLL is off).
 *
 * 1: HSE is fed into PLL without dividing the frequency.
 * 0: HSI is not divided and sent to PLL.
 */
#define CFGR0_PLLSRC_Mask ((uint32_t)0x00010000)

/*
 * AHB clock source prescaler control.
 *
 * 0000: Prescaler off.
 * 0001: SYSCLK divided by 2.
 * 0010: SYSCLK divided by 3.
 * 0011: SYSCLK divided by 4.
 * 0100: SYSCLK divided by 5.
 * 0101: SYSCLK divided by 6.
 * 0110: SYSCLK divided by 7.
 * 0111: SYSCLK divided by 8.
 * 1000: SYSCLK divided by 2.
 * 1001: SYSCLK divided by 4.
 * 1010: SYSCLK divided by 8.
 * 1011: SYSCLK divided by 16.
 * 1100: SYSCLK divided by 32.
 * 1101: SYSCLK divided by 64.
 * 1110: SYSCLK divided by 128.
 * 1111: SYSCLK divided by 256.
 *
 * Note: When the prescaler factor of the AHB clock source is greater than 1,
 * the prefetch buffer must be turned on.
 */
#define CFGR0_HPRE_Set_Mask ((uint32_t)0x000000F0)

/* ADC clock source prescaler control {13:11,15:14}.
 *
 * 000xx: AHBCLK divided by 2 as ADC clock.
 * 010xx: AHBCLK divided by 4 as ADC clock.
 * 100xx: AHBCLK divided by 6 as ADC clock.
 * 110xx: AHBCLK divided by 8 as ADC clock.
 * 00100: AHBCLK divided by 4 as ADC clock.
 * 01100: AHBCLK divided by 8 as ADC clock.
 * 10100: AHBCLK divided by 12 as ADC clock.
 * 11100: AHBCLK divided by 16 as ADC clock.
 * 00101: AHBCLK divided by 8 as ADC clock.
 * 01101: AHBCLK divided by 16 as ADC clock.
 * 10101: AHBCLK divided by 24 as ADC clock.
 * 11101: AHBCLK divided by 32 as ADC clock.
 * 00110: AHBCLK divided by 16 as ADC clock.
 * 01110: AHBCLK divided by 32 as ADC clock.
 * 10110: AHBCLK divided by 48 as ADC clock.
 * 11110: AHBCLK divided by 64 as ADC clock.
 * 00111: AHBCLK divided by 32 as ADC clock.
 * 01111: AHBCLK divided by 64 as ADC clock.
 * 10111: AHBCLK divided by 96 as ADC clock.
 * 11111: AHBCLK divided by 128 as ADC clock.
 *
 * Note: The ADC clock should not exceed a maximum of 24MHz.
 */
#define CFGR0_ADCPRE_Set_Mask ((uint32_t)0x0000F800)

/* system_private_function_proto_types */
void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

static void SetSysClock(void);
static void SetSysClockTo_8MHz_HSI(void);
static void SetSysClockTo_24MHZ_HSI(void);
static void SetSysClockTo_48MHZ_HSI(void);
static void SetSysClockTo_8MHz_HSE(void);
static void SetSysClockTo_24MHz_HSE(void);
static void SetSysClockTo_48MHz_HSE(void);


/*********************************************************************
 * @fn      SystemInit
 *
 * @brief   Setup the microcontroller system Initialize the Embedded Flash
 * Interface, the PLL and update the SystemCoreClock variable.
 *
 * @return  none
 */
void sys_init(void *config)
{
    RCC->CTLR |= (uint32_t)0x00000001;
    RCC->CFGR0 &= (uint32_t)0xFCFF0000;
    RCC->CTLR &= (uint32_t)0xFEF6FFFF;
    RCC->CTLR &= (uint32_t)0xFFFBFFFF;
    RCC->CFGR0 &= (uint32_t)0xFFFEFFFF;
    RCC->INTR = 0x009F0000;
}


/*********************************************************************
 * @fn      SystemCoreClockUpdate
 *
 * @brief   Update SystemCoreClock variable according to Clock Register Values.
 *
 * @return  none
 */
void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0, pllsource = 0;

    tmp = RCC->CFGR0 & RCC_SWS;

    switch (tmp) {
    case 0x00:
        SystemCoreClock = HSI_VALUE;
        break;
    case 0x04:
        SystemCoreClock = HSE_VALUE;
        break;
    case 0x08:
        pllsource = RCC->CFGR0 & RCC_PLLSRC;
        if (pllsource == 0x00) {
            SystemCoreClock = HSI_VALUE * 2;
        } else {
            SystemCoreClock = HSE_VALUE * 2;
        }
        break;
    default:
        SystemCoreClock = HSI_VALUE;
        break;
    }

    tmp = AHBPrescTable[((RCC->CFGR0 & RCC_HPRE) >> 4)];

    if (((RCC->CFGR0 & RCC_HPRE) >> 4) < 8) {
        SystemCoreClock /= tmp;
    } else {
        SystemCoreClock >>= tmp;
    }
}


/*********************************************************************
 * @fn      SetSysClock
 *
 * @brief   Configures the System clock frequency, HCLK, PCLK2 and PCLK1
 * prescalers.
 *
 * @return  none
 */
static void sys_set_clock(void *clock)
{
    enum ch32v003_clock_source sc = *(enum ch32v003_clock_source *)clock;

    switch (sc) {
    case SYSCLK_FREQ_8MHz_HSI:
        SystemCoreClock = 8000000;
        SetSysClockTo_8MHz_HSI();
        break;
    case SYSCLK_FREQ_24MHZ_HSI:
        SystemCoreClock = HSI_VALUE;
        SetSysClockTo_24MHZ_HSI();
        break;
    case SYSCLK_FREQ_48MHZ_HSI:
        SystemCoreClock = 48000000;
        SetSysClockTo_48MHZ_HSI();
        break;
    case SYSCLK_FREQ_8MHz_HSE:
        SystemCoreClock = 8000000;
        SetSysClockTo_8MHz_HSE();
        break;
    case SYSCLK_FREQ_24MHz_HSE:
        SystemCoreClock = HSE_VALUE;
        SetSysClockTo_24MHz_HSE();
        break;
    case SYSCLK_FREQ_48MHz_HSE:
        SystemCoreClock = 48000000;
        SetSysClockTo_48MHz_HSE();
        break;
    }

    /* if none of the define above is enabled, the hsi is used as system clock
     * source (default after reset)
     */
}


/*********************************************************************
 * @fn      SetSysClockTo_8MHz_HSI
 *
 * @brief   Sets HSE as System clock source and configure HCLK, PCLK2 and PCLK1
 * prescalers.
 *
 * @return  none
 */
static void SetSysClockTo_8MHz_HSI(void)
{
    /* Flash 0 wait state */
    FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);
    FLASH->ACTLR |= (uint32_t)FLASH_ACTLR_LATENCY_0;

    /* HCLK = SYSCLK = APB1 */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV3;
}


/*********************************************************************
 * @fn      SetSysClockTo_24MHZ_HSI
 *
 * @brief   Sets System clock frequency to 24MHz and configure HCLK, PCLK2 and
 * PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo_24MHZ_HSI(void)
{
    /* Flash 0 wait state */
    FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);
    FLASH->ACTLR |= (uint32_t)FLASH_ACTLR_LATENCY_0;

    /* HCLK = SYSCLK = APB1 */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
}


/*********************************************************************
 * @fn      SetSysClockTo_48MHZ_HSI
 *
 * @brief   Sets System clock frequency to 48MHz and configure HCLK, PCLK2 and
 * PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo_48MHZ_HSI(void)
{
    /* Flash 0 wait state */
    FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);
    FLASH->ACTLR |= (uint32_t)FLASH_ACTLR_LATENCY_1;

    /* HCLK = SYSCLK = APB1 */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;

    /* PLL configuration: PLLCLK = HSI * 2 = 48 MHz */
    RCC->CFGR0 &= (uint32_t)((uint32_t) ~(RCC_PLLSRC));
    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSI_Mul2);

    /* Enable PLL */
    RCC->CTLR |= RCC_PLLON;
    /* Wait till PLL is ready */
    while ((RCC->CTLR & RCC_PLLRDY) == 0) {
    }
    /* Select PLL as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t) ~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08) {
    }
}


/*********************************************************************
 * @fn      SetSysClockTo_8MHz_HSE
 *
 * @brief   Sets System clock frequency to 56MHz and configure HCLK, PCLK2 and
 * PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo_8MHz_HSE(void)
{
    _rw uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* Close PA0-PA1 GPIO function */
    RCC->APB2PCENR |= RCC_AFIOEN;
    AFIO->PCFR1 |= (1 << 15);

    RCC->CTLR |= ((uint32_t)RCC_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSEStatus = RCC->CTLR & RCC_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    RCC->APB2PCENR |= RCC_AFIOEN;
    AFIO->PCFR1 |= (1 << 15);

    if ((RCC->CTLR & RCC_HSERDY) != 0) {
        HSEStatus = (uint32_t)0x01;
    } else {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01) {
        /* Flash 0 wait state */
        FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);
        FLASH->ACTLR |= (uint32_t)FLASH_ACTLR_LATENCY_0;

        /* HCLK = SYSCLK = APB1 */
        RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV3;

        /* Select HSE as system clock source */
        RCC->CFGR0 &= (uint32_t)((uint32_t) ~(RCC_SW));
        RCC->CFGR0 |= (uint32_t)RCC_SW_HSE;
        /* Wait till HSE is used as system clock source */
        while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x04) {
        }
    } else {
        /*
         * If HSE fails to start-up, the application will have wrong clock
         * configuration. User can add here some code to deal with this error
         */
    }
}


/*********************************************************************
 * @fn      SetSysClockTo_24MHz_HSE
 *
 * @brief   Sets System clock frequency to 72MHz and configure HCLK, PCLK2 and
 * PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo_24MHz_HSE(void)
{
    _rw uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* Close PA0-PA1 GPIO function */
    RCC->APB2PCENR |= RCC_AFIOEN;
    AFIO->PCFR1 |= (1 << 15);

    RCC->CTLR |= ((uint32_t)RCC_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSEStatus = RCC->CTLR & RCC_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    RCC->APB2PCENR |= RCC_AFIOEN;
    AFIO->PCFR1 |= (1 << 15);

    if ((RCC->CTLR & RCC_HSERDY) != 0) {
        HSEStatus = (uint32_t)0x01;
    } else {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01) {
        /* Flash 0 wait state */
        FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);
        FLASH->ACTLR |= (uint32_t)FLASH_ACTLR_LATENCY_0;

        /* HCLK = SYSCLK = APB1 */
        RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;

        /* Select HSE as system clock source */
        RCC->CFGR0 &= (uint32_t)((uint32_t) ~(RCC_SW));
        RCC->CFGR0 |= (uint32_t)RCC_SW_HSE;
        /* Wait till HSE is used as system clock source */
        while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x04) {
        }
    } else {
        /*
         * If HSE fails to start-up, the application will have wrong clock
         * configuration. User can add here some code to deal with this error
         */
    }
}


/*********************************************************************
 * @fn      SetSysClockTo_48MHz_HSE
 *
 * @brief   Sets System clock frequency to 72MHz and configure HCLK, PCLK2 and
 * PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClockTo_48MHz_HSE(void)
{
    _rw uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* Close PA0-PA1 GPIO function */
    RCC->APB2PCENR |= RCC_AFIOEN;
    AFIO->PCFR1 |= (1 << 15);

    RCC->CTLR |= ((uint32_t)RCC_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSEStatus = RCC->CTLR & RCC_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CTLR & RCC_HSERDY) != 0) {
        HSEStatus = (uint32_t)0x01;
    } else {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01) {
        /* Flash 0 wait state */
        FLASH->ACTLR &= (uint32_t)((uint32_t)~FLASH_ACTLR_LATENCY);
        FLASH->ACTLR |= (uint32_t)FLASH_ACTLR_LATENCY_1;

        /* HCLK = SYSCLK = APB1 */
        RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;

        /* PLL configuration: PLLCLK = HSE * 2 = 48 MHz */
        RCC->CFGR0 &= (uint32_t)((uint32_t) ~(RCC_PLLSRC));
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE_Mul2);

        /* Enable PLL */
        RCC->CTLR |= RCC_PLLON;
        /* Wait till PLL is ready */
        while ((RCC->CTLR & RCC_PLLRDY) == 0) {
        }
        /* Select PLL as system clock source */
        RCC->CFGR0 &= (uint32_t)((uint32_t) ~(RCC_SW));
        RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08) {
        }
    } else {
        /*
         * If HSE fails to start-up, the application will have wrong clock
         * configuration. User can add here some code to deal with this error
         */
    }
}

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void) {}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
    while (1) {
    }
}

static uint32_t sys_get_clock(void *unit)
{
    uint32_t           tmp = 0, pllsource = 0, presc = 0;
    RCC_ClocksTypeDef *RCC_Clocks;

    RCC_Clocks = (RCC_ClocksTypeDef *)unit;

    tmp = RCC->CFGR0 & CFGR0_SWS_Mask;

    switch (tmp) {
    case 0x00:
        RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
        break;

    case 0x04:
        RCC_Clocks->SYSCLK_Frequency = HSE_VALUE;
        break;

    case 0x08:
        pllsource = RCC->CFGR0 & CFGR0_PLLSRC_Mask;

        if (pllsource == 0x00) {
            RCC_Clocks->SYSCLK_Frequency = HSI_VALUE * 2;
        } else {
            RCC_Clocks->SYSCLK_Frequency = HSE_VALUE * 2;
        }
        break;

    default:
        RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
        break;
    }

    tmp   = RCC->CFGR0 & CFGR0_HPRE_Set_Mask;
    tmp   = tmp >> 4;
    presc = AHBPrescTable[tmp];

    if (((RCC->CFGR0 & CFGR0_HPRE_Set_Mask) >> 4) < 8) {
        RCC_Clocks->HCLK_Frequency = RCC_Clocks->SYSCLK_Frequency / presc;
    } else {
        RCC_Clocks->HCLK_Frequency = RCC_Clocks->SYSCLK_Frequency >> presc;
    }

    RCC_Clocks->PCLK1_Frequency = RCC_Clocks->HCLK_Frequency;
    RCC_Clocks->PCLK2_Frequency = RCC_Clocks->HCLK_Frequency;
    tmp                         = RCC->CFGR0 & CFGR0_ADCPRE_Set_Mask;
    tmp                         = tmp >> 11;

    if ((tmp & 0x13) >= 4) {
        tmp -= 12;
    }

    presc                        = ADCPrescTable[tmp];
    RCC_Clocks->ADCCLK_Frequency = RCC_Clocks->PCLK2_Frequency / presc;
}

eer_sys_handler_t eer_hw_sys = {
    .init = sys_init,
    .clock = {
        .set = sys_set_clock,
        .get = sys_get_clock,
    },
};
