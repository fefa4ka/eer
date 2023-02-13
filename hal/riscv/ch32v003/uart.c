#include "uart.h"

extern eer_sys_handler_t eer_hw_sys;

/* USART Enable Mask */
#define CTLR1_UE_Set ((uint16_t)0x2000)
/* CTLR1 register Mask */
#define CTLR1_CLEAR_Mask ((uint32_t)0xFFF0FEFF)

/* USART OverSampling-8 Mask */
#define CTLR1_OVER8_Set   ((uint16_t)0x8000) /* USART OVER8 mode Enable Mask */
#define CTLR1_OVER8_Reset ((uint16_t)0x7FFF) /* USART OVER8 mode Disable Mask  \
                                              */
/* USART CR2 STOP Bits Mask */
#define CTLR2_STOP_CLEAR_Mask ((uint16_t)0xCFFF)

/* USART CR3 Mask */
#define CTLR3_CLEAR_Mask ((uint16_t)0xFCFF)

#define USART1_BASE (APB2PERIPH_BASE + 0x3800)
#define USART1      ((USART_TypeDef *)USART1_BASE)

#define RCC_APB2Periph_GPIOD  ((uint32_t)0x00000020)
#define RCC_APB2Periph_USART1 ((uint32_t)0x00004000)

/* USART_Word_Length */
#define USART_WordLength_8b ((uint16_t)0x0000)
#define USART_WordLength_9b ((uint16_t)0x1000)

/* USART_Stop_Bits */
#define USART_StopBits_1   ((uint16_t)0x0000)
#define USART_StopBits_0_5 ((uint16_t)0x1000)
#define USART_StopBits_2   ((uint16_t)0x2000)
#define USART_StopBits_1_5 ((uint16_t)0x3000)

/* USART_Parity */
#define USART_Parity_No   ((uint16_t)0x0000)
#define USART_Parity_Even ((uint16_t)0x0400)
#define USART_Parity_Odd  ((uint16_t)0x0600)

/* USART_Mode */
#define USART_Mode_Rx ((uint16_t)0x0004)
#define USART_Mode_Tx ((uint16_t)0x0008)

/* USART_Hardware_Flow_Control */
#define USART_HardwareFlowControl_None    ((uint16_t)0x0000)
#define USART_HardwareFlowControl_RTS     ((uint16_t)0x0100)
#define USART_HardwareFlowControl_CTS     ((uint16_t)0x0200)
#define USART_HardwareFlowControl_RTS_CTS ((uint16_t)0x0300)

/* USART_Clock */
#define USART_Clock_Disable ((uint16_t)0x0000)
#define USART_Clock_Enable  ((uint16_t)0x0800)

/* USART_Clock_Polarity */
#define USART_CPOL_Low  ((uint16_t)0x0000)
#define USART_CPOL_High ((uint16_t)0x0400)

/* USART_Clock_Phase */
#define USART_CPHA_1Edge ((uint16_t)0x0000)
#define USART_CPHA_2Edge ((uint16_t)0x0200)

/* USART_Last_Bit */
#define USART_LastBit_Disable ((uint16_t)0x0000)
#define USART_LastBit_Enable  ((uint16_t)0x0100)

/* USART_Interrupt_definition */
#define USART_IT_PE     ((uint16_t)0x0028)
#define USART_IT_TXE    ((uint16_t)0x0727)
#define USART_IT_TC     ((uint16_t)0x0626)
#define USART_IT_RXNE   ((uint16_t)0x0525)
#define USART_IT_ORE_RX ((uint16_t)0x0325)
#define USART_IT_IDLE   ((uint16_t)0x0424)
#define USART_IT_LBD    ((uint16_t)0x0846)
#define USART_IT_CTS    ((uint16_t)0x096A)
#define USART_IT_ERR    ((uint16_t)0x0060)
#define USART_IT_ORE_ER ((uint16_t)0x0360)
#define USART_IT_NE     ((uint16_t)0x0260)
#define USART_IT_FE     ((uint16_t)0x0160)

#define USART_IT_ORE USART_IT_ORE_ER

/* USART_DMA_Requests */
#define USART_DMAReq_Tx ((uint16_t)0x0080)
#define USART_DMAReq_Rx ((uint16_t)0x0040)

/* USART_WakeUp_methods */
#define USART_WakeUp_IdleLine    ((uint16_t)0x0000)
#define USART_WakeUp_AddressMark ((uint16_t)0x0800)

/* USART_LIN_Break_Detection_Length */
#define USART_LINBreakDetectLength_10b ((uint16_t)0x0000)
#define USART_LINBreakDetectLength_11b ((uint16_t)0x0020)

/* USART_IrDA_Low_Power */
#define USART_IrDAMode_LowPower ((uint16_t)0x0004)
#define USART_IrDAMode_Normal   ((uint16_t)0x0000)

/* USART_Flags */
#define USART_FLAG_CTS  ((uint16_t)0x0200)
#define USART_FLAG_LBD  ((uint16_t)0x0100)
#define USART_FLAG_TXE  ((uint16_t)0x0080)
#define USART_FLAG_TC   ((uint16_t)0x0040)
#define USART_FLAG_RXNE ((uint16_t)0x0020)
#define USART_FLAG_IDLE ((uint16_t)0x0010)
#define USART_FLAG_ORE  ((uint16_t)0x0008)
#define USART_FLAG_NE   ((uint16_t)0x0004)
#define USART_FLAG_FE   ((uint16_t)0x0002)
#define USART_FLAG_PE   ((uint16_t)0x0001)


static uint32_t sys_clock(void) { return 0; }

static uint16_t uart_baudrate(uint32_t baudrate)
{
    uint32_t x;


    return (uint16_t)x;
}

 /*
 * USART pins  │  Configuration                  │  GPIO configuration
 * ────────────│─────────────────────────────────│───────────────────────────────►
 * USARTx_TX   │  Full-duplex mode               │  Push-pull multiplexed outputs
 *             │  Half-duplex synchronous mode   │  Push-pull multiplexed outputs
 * USARTx_RX   │  Full-duplex mode               │  Floating input or pull-up input
 *             │  Half-duplex synchronous mode   │  Not used
 * USARTx_CK   │  Synchronous mode               │  Push-pull multiplexed output
 * USARTx_RTS  │  Hardware flow control          │  Push-pull multiplexed output
 * USARTx_CTS  │  Hardware flow control          │  Floating input or pull-up input
 */
static void uart_init(void *baudrate_ptr)
{
    uint32_t config = 0x00, apbclock = 0x00;
    uint32_t integerdivider    = 0x00;
    uint32_t fractionaldivider = 0x00;
    uint32_t usartxbase        = 0;
    uint32_t baudrate          = *(uint32_t *)baudrate_ptr;

    /*
     * 7.3.1.1 Port configuration register
     * (GPIOx_CFGLx)
     * 10: Multiplexed function push-pull output mode.
     * 11: Multiplexing function open-drain output mode.
     * */
    RCC_ClocksTypeDef RCC_ClocksStatus;

    eer_pin_t tx = eer_hw_pin(D, 5);
    eer_pin_t rx = eer_hw_pin(D, 6);

    /* Enables or disables the High Speed APB (APB2) peripheral clock. */
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;

    gpio_init(&tx, PIN_FUNC_PUSHPULL, GPIO_Speed_50MHz);
    gpio_init(&rx, PIN_MODE_FLOAT, 0);

    eer_hw_sys.clock.get(&RCC_ClocksStatus);

    usartxbase = (uint32_t)USART1;
    config     = USART1->CTLR2;
    config &= CTLR2_STOP_CLEAR_Mask;
    config |= (uint32_t)USART_StopBits_1;

    USART1->CTLR2 = (uint16_t)config;
    config        = USART1->CTLR1;
    config &= CTLR1_CLEAR_Mask;
    config |= (uint32_t)USART_WordLength_8b | USART_Parity_No | USART_Mode_Tx
              | USART_Mode_Rx;
    USART1->CTLR1 = (uint16_t)config;

    config = USART1->CTLR3;
    config &= CTLR3_CLEAR_Mask;
    config |= USART_HardwareFlowControl_None;
    USART1->CTLR3 = (uint16_t)config;

    apbclock = RCC_ClocksStatus.PCLK2_Frequency;

    /* This member configures the USART communication baud rate.
     * The baud rate is computed using the following formula:
     * IntegerDivider = ((PCLKx) / (16 * (USART_InitStruct->USART_BaudRate)))
     * FractionalDivider = ((IntegerDivider - ((u32) IntegerDivider)) * 16) +
     * 0.5
     */
    integerdivider = (25 * apbclock) / (2 * baudrate);
    if ((USART1->CTLR1 & CTLR1_OVER8_Set) == 0) {
        integerdivider = integerdivider / 2;
    }

    config = (integerdivider / 100) << 4;

    fractionaldivider = integerdivider - (100 * (config >> 4));
    fractionaldivider = fractionaldivider * 8;
    if ((USART1->CTLR1 & CTLR1_OVER8_Set) == 0) {
        fractionaldivider = fractionaldivider * 2;
    }
    config |= (((fractionaldivider + 50) / 100)) & ((uint8_t)0x0F);

    USART1->BRR = (uint16_t)config;

    /* Enable UART */
    USART1->CTLR1 |= CTLR1_UE_Set;
}

/* Read data register non-empty flag, this bit is set by hardware when data in
 * the shift register is transferred to the data register. If RXNEIE is already
 * set, a corresponding interrupt is also generated. A read operation of the
 * data register clears this bit. It is also possible to clear the bit by
 * writing a 0 directly.
 *
 * 1: Data received and able to be read out.
 * 0: The data has not been received.
 */
static bool uart_is_data_received() { return USART1->STATR & USART_FLAG_RXNE; }

/* Send data register empty flag. This bit is set by hardware when the data in
 * the TDR register is transferred to the shift register by hardware. If TXEIE
 * is already set, an interrupt will be generated to perform a write operation
 * to the data register and this bit will be reset.
 *
 * 1: the data has been transferred to the shift register.
 * 0: The data has not been transferred to the shift register.
 */
static bool uart_is_transmit_ready() { return USART1->STATR & USART_FLAG_TXE; }

static void uart_transmit(uint8_t data)
{
    /* TODO: check just uint8_t */
    uint16_t prepared = (uint16_t)data;
    USART1->DATAR     = (prepared & (uint16_t)0x01FF);
}


static uint8_t uart_receive()
{
    /* TODO: check uint16_t */
    return (uint8_t)(USART1->DATAR & (uint16_t)0x01FF);
    ;
}


eer_serial_handler_t eer_hw_uart = {.init              = uart_init,
                                    .is_data_received  = uart_is_data_received,
                                    .is_transmit_ready = uart_is_transmit_ready,
                                    .transmit          = uart_transmit,
                                    .receive           = uart_receive};
