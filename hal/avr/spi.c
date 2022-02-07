#include "avr.h"

#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
    #define SPI_DDR  DDRB
    #define SPI_PORT PORTB
    #define SPI_MISO PORTB4
    #define SPI_MOSI PORTB3
    #define SPI_SCK  PORTB5
    #define SPI_SS   PORTB2
#endif

static void spi_init(void *config)
{
    // make the MOSI, SCK, and SS pins outputs
    SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS);

    // make sure the MISO pin is input
    SPI_DDR &= ~(1 << SPI_MISO);

    // set up the SPI module: SPI enabled, MSB first, master mode,
    //  clock polarity and phase = 0, F_osc/16
    SPCR = (1 << SPE) | (1 << MSTR); // | ( 1 << SPI_SPR0 );
    SPSR = 1; // set double SPI speed for F_osc/2
}

static bool spi_is_data_received()
{
    if (!(SPSR & (1 << SPIF))) {
        SPDR = 0xFF;
    } else {
        return true;
    }
}

static bool spi_is_transmit_ready() { return !(SPSR & (1 << SPIF)); }

static void spi_transmit(uint8_t data) { SPDR = data; }

uint8_t spi_receive() { return SPDR; }
