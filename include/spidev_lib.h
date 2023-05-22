#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <unistd.h>
#include <linux/const.h>
#include "bcm2835.h"


typedef enum
{
    SPIDEV_AUX_SPI_CS0 = 0,		/*!< Chip Select 0 */  // PIN 12, GPIO 18
    SPIDEV_AUX_SPI_CS1 = 1,		/*!< Chip Select 1 */  // PIN 11, GPIO 17
    SPIDEV_AUX_SPI_CS2 = 2,		/*!< Chip Select 2 */  // PIN 36, GPIO 16
    SPIDEV_AUX_SPI_CS_NONE = 3		/*!< No CS, control it yourself */
} SPIDEVAUXSPIChipSelect;

#define	SPI_CPHA		_BITUL(0)	/* clock phase */
#define	SPI_CPOL		_BITUL(1)	/* clock polarity */

#define	SPI_MODE_0		(0|0)		/* (original MicroWire) */
#define	SPI_MODE_1		(0|SPI_CPHA)
#define	SPI_MODE_2		(SPI_CPOL|0)
#define	SPI_MODE_3		(SPI_CPOL|SPI_CPHA)
#define	SPI_MODE_X_MASK		(SPI_CPOL|SPI_CPHA)

#define	SPI_CS_HIGH		_BITUL(2)	/* chipselect active high? */
#define	SPI_LSB_FIRST		_BITUL(3)	/* per-word bits-on-wire */
#define	SPI_3WIRE		_BITUL(4)	/* SI/SO signals shared */
#define	SPI_LOOP		_BITUL(5)	/* loopback mode */
#define	SPI_NO_CS		_BITUL(6)	/* 1 dev/bus, no chipselect */
#define	SPI_READY		_BITUL(7)	/* slave pulls low to pause */
#define	SPI_TX_DUAL		_BITUL(8)	/* transmit with 2 wires */
#define	SPI_TX_QUAD		_BITUL(9)	/* transmit with 4 wires */
#define	SPI_RX_DUAL		_BITUL(10)	/* receive with 2 wires */
#define	SPI_RX_QUAD		_BITUL(11)	/* receive with 4 wires */
#define	SPI_CS_WORD		_BITUL(12)	/* toggle cs after each word */
#define	SPI_TX_OCTAL		_BITUL(13)	/* transmit with 8 wires */
#define	SPI_RX_OCTAL		_BITUL(14)	/* receive with 8 wires */
#define	SPI_3WIRE_HIZ		_BITUL(15)	/* high impedance turnaround */

/*
 * All the bits defined above should be covered by SPI_MODE_USER_MASK.
 * The SPI_MODE_USER_MASK has the SPI_MODE_KERNEL_MASK counterpart in
 * 'include/linux/spi/spi.h'. The bits defined here are from bit 0 upwards
 * while in SPI_MODE_KERNEL_MASK they are from the other end downwards.
 * These bits must not overlap. A static assert check should make sure of that.
 * If adding extra bits, make sure to increase the bit index below as well.
 */
#define SPI_MODE_USER_MASK	(_BITUL(16) - 1)

/* Define file path of SPIs */
#define SPI_DEVICE0_CS0          "/dev/spidev0.0"
#define SPI_DEVICE0_CS1          "/dev/spidev0.1"

#define SPI_DEVICE6_CS0          "/dev/spidev6.0"
// #define SPI_DEVICE6_CS1          "/dev/spidev6.1"
// #define SPI_DEVICE1_CS2          "/dev/spidev1.2"

typedef struct {
    const char *device;
    uint32_t mode;
    uint8_t bits;
    uint32_t speed;
    uint16_t delay;
    uint8_t cs_change;
    int verbose;
}spi_init_params;

typedef struct {
    spi_init_params *init;
    int fd;
}spi_device;


int spidev_transfer(spi_device *sdev, uint8_t const *tx, uint8_t const *rx, size_t len);
int spidev_transfernb(spi_device *sdev, uint8_t const *tx, size_t len);
int spidev_init(spi_device *sdev);
int spidev_free(spi_device *dev);

int Aux_Spi_Init(spi_device **device, uint32_t spi_speed, uint8_t spi_mode, uint16_t cs_delay_us, SPIDEVAUXSPIChipSelect cs_pin);
int Aux_Spi_Free(spi_device *device);

#endif