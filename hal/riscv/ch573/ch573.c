#include "ch573.h"


uint32_t irq_status;

void eer_hw_isr_enable()
{
    /* Recover Interrupt Enable Setting Register */
    PFIC->IENR[0] = (irq_status << 8);
    PFIC->IENR[1] = (irq_status >> 24);
}

void eer_hw_isr_disable()
{
    /* Save current Interrupt Enable Status Register */
    irq_status = (PFIC->ISR[0] >> 8) | (PFIC->ISR[1] << 24);

    /* Disable All IRQ
     * by setting Interrupt Enable Clear Register */
    PFIC->IRER[0] = 0xffffffff;
    PFIC->IRER[1] = 0xffffffff;
}
