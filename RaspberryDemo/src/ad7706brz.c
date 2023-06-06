#include "bcm2835.h"
#include <stdio.h>
#include "ad7706brz.h"


void ad7706_init()
{
    bcm2835_gpio_fsel(RESET_PIN, BCM2835_GPIO_FSEL_OUTP);  // Set reset pin as output
    bcm2835_gpio_write(RESET_PIN, HIGH);  // Set reset pin high to enable AD7706BRZ
    
    bcm2835_gpio_fsel(DRDY_PIN, BCM2835_GPIO_FSEL_INPT);  // Set data ready pin as input
    
    bcm2835_spi_begin();  // Initialize SPI
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);  // Set bit order
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);  // Set SPI mode
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);  // Set clock divider
}

uint16_t ad7706_read(uint8_t channel)
{
    uint8_t tx_buf[3] = {0, 0, 0};
    uint8_t rx_buf[3] = {0, 0, 0};
    
    tx_buf[0] = 0x38 | channel << 4;  // Set command byte for configuration register
    tx_buf[1] = 0b10100000;  // Set configuration register for bipolar input range of +/- 2.5V
    tx_buf[2] = 0;
    
    bcm2835_gpio_write(CS_PIN, LOW);  // Set chip select pin low to enable AD7706BRZ
    
    bcm2835_spi_transfern(tx_buf, 3);  // Send configuration command
    
    while (bcm2835_gpio_lev(DRDY_PIN) == HIGH) {}  // Wait until data is ready
    
    bcm2835_spi_transfern(rx_buf, 3);  // Read conversion result
    
    bcm2835_gpio_write(CS_PIN, HIGH);  // Set chip select pin high to disable AD7706BRZ
    
    uint16_t value = (rx_buf[1] << 8) | rx_buf[2];  // Combine MSB and LSB into 16-bit value
    
    return value;
}

/*
int main()
{
    if (!bcm2835_init())  // Initialize bcm2835 library
        return 1;
    
    ad7706_init();  // Initialize AD7706BRZ
    
    while (1) {
        uint16_t value = ad7706_read(0);  // Read from channel 0
        printf("Current: %.2f mA\n", (float)value * 5.0 / 32767.0);  // Convert value to current in mA
    }
    
    bcm2835_spi_end();  // Close SPI
    bcm2835_close();  // Close bcm2835 library
    
    return 0;
}*/
