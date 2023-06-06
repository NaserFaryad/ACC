#include <bcm2835.h>
#include <stdio.h>
#include "ad9833_test.h"
#include <math.h>


void write_ad9833(unsigned int data)
{
    // send the 16-bit data to the AD9833 chip
    bcm2835_spi_transfer(data >> 8); // high byte first
    bcm2835_spi_transfer(data & 0xFF); // low byte second
}

void set_ad9833_frequency(double freq)
{
    // calculate the frequency register value
    unsigned int regval = (unsigned int)(freq * pow(2, 28) / AD9833_CLK_FREQ);

    // select frequency register 0 and write the value
    write_ad9833(AD9833_REG_FREQ0 | regval);

    // select frequency register 1 and write the same value
    write_ad9833(AD9833_REG_FREQ1 | regval);
}

void set_ad9833_phase(double phase)
{
    // calculate the phase register value
    unsigned int regval = (unsigned int)(phase * pow(2, 12) / 360);

    // select phase register 0 and write the value
    write_ad9833(AD9833_REG_PHASE0 | regval);

    // select phase register 1 and write the same value
    write_ad9833(AD9833_REG_PHASE1 | regval);
}

void init_ad9833()
{
    // set up the SPI interface parameters
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

    // configure the AD9833 control register
    write_ad9833(AD9833_CMD_RESET | AD9833_CMD_SLEEP12 | AD9833_CMD_OPBITEN);

    // set the initial frequency and phase values
    set_ad9833_frequency(1000.0);
    set_ad9833_phase(0.0);

    // enable the output
    write_ad9833(AD9833_CMD_NOP | AD9833_CMD_FSEL0 | AD9833_CMD_PSEL0);
}

/*
int main(int argc, char **argv)
{
    // initialize the bcm2835 library
    if (!bcm2835_init())
    {
        printf("Error: bcm2835 library not initialized.\n");
        return 1;
    }

    // initialize the AD9833 chip
    init_ad9833();

    // generate a square wave with a frequency of 1000 Hz and duty cycle of 50%
    while (1)
    {
        set_ad9833_frequency(1000.0);
        set_ad9833_phase(0.0);
        bcm2835_delay(500);
        set_ad9833_frequency(1000.0);
        set_ad9833_phase(180.0);
        bcm2835_delay(500);
    }

    // shut down the bcm2835 library
    bcm2835_close();
    return 
}*/
