#include <stdlib.h>

#define CS_PIN RPI_GPIO_P1_24   // Chip select pin for SPI
#define RESET_PIN RPI_GPIO_P1_22  // Reset pin for AD7706BRZ
#define DRDY_PIN RPI_GPIO_P1_18   // Data ready pin for AD7706BRZ

void ad7706_init();
uint16_t ad7706_read(uint8_t channel);
