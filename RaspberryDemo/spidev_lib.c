#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "spidev_lib.h"


int spidev_transfer(spi_device *sdev, uint8_t const *tx, uint8_t const *rx, size_t len)
{
        int ret;
        struct spi_ioc_transfer tr = {
                .tx_buf = (unsigned long)tx,
                .rx_buf = (unsigned long)rx,
                .len = len,
                .delay_usecs = sdev->init->delay,
                .speed_hz = sdev->init->speed,
                .bits_per_word = sdev->init->bits,
                .cs_change = sdev->init->cs_change,
        };
        // printf("LEEEEN: %d ---- TX before sent: %02X %02X\n", len, tx[0], tx[1]);
        ret = ioctl(sdev->fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret == -1)
        return ret;

    // printf("TX: %02X %02X %02X\n", tx[0], tx[1], tx[2]);
    // printf("RX: %02X %02X %02X\n", rx[0], rx[1], rx[2]);
    return ret;
}

int spidev_transfernb(spi_device *sdev, uint8_t const *tx, size_t len) {
    int ret = 0;
    ret = spidev_transfer(sdev, tx, tx, len);
    return ret;
}


int spidev_init(spi_device *sdev) {
    int ret = 0;
        int fd;

        fd = open(sdev->init->device, O_RDWR);
        if (fd < 0)
                return ret;

        /*
         * spi mode
         */
        ret = ioctl(fd, SPI_IOC_WR_MODE32, &sdev->init->mode);
        if (ret == -1)
        return ret;

        /* RD is read what mode the device actually is in */
        ret = ioctl(fd, SPI_IOC_RD_MODE32, &sdev->init->mode);
        if (ret == -1)
                return ret;

        /*
         * bits per word
         */
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &sdev->init->bits);
        if (ret == -1)
                return ret;

        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &sdev->init->bits);
        if (ret == -1)
                return ret;

        /*
         * max speed hz
         */
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &sdev->init->speed);
        if (ret == -1)
                return ret;

        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &sdev->init->speed);
        if (ret == -1)
                return ret;

        printf("spi mode: 0x%x\n", sdev->init->mode);
        printf("bits per word: %u\n", sdev->init->bits);
        printf("max speed: %u Hz (%u kHz)\n", sdev->init->speed, sdev->init->speed/1000);
        printf("\n\n\n\n\n\n");

    sdev -> fd = fd;

        return ret;
}

int spidev_free(spi_device *dev) {
    close(dev->fd);
    free(dev->init);
    free(dev);
    return 0;
}

/*
        interface function for spidev, this function should be used to initialize the spi 6
*/
int Aux_Spi_Init(spi_device **device, uint32_t spi_speed, uint8_t spi_mode, uint16_t cs_delay_us, SPIDEVAUXSPIChipSelect cs_pin) {
    if (!bcm2835_init())
    {
        fprintf(stderr,"bcm2835_init failed. Are you running as root??\n");
        return -1;
    }


        bcm2835_gpio_fsel(RPI_V2_GPIO_P1_35, BCM2835_GPIO_FSEL_ALT3);  // set pin 35 to MISO
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_38, BCM2835_GPIO_FSEL_ALT3);  // set pin 38 to MOSI
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_40, BCM2835_GPIO_FSEL_ALT3);  // set pin 40 to SCLK
    /* Initialize the SPI communication. */
        spi_device *sdev = (spi_device*)  malloc(sizeof(spi_device));
    sdev->init = (spi_init_params*) malloc(sizeof(spi_init_params));
    sdev->init->speed = spi_speed;
    sdev->init->bits = 8;
    sdev->init->delay = 0;
        sdev->init->cs_change = 0;
    sdev->init->mode = spi_mode;

    switch (cs_pin) {
        case (SPIDEV_AUX_SPI_CS0):
            sdev->init->device = SPI_DEVICE6_CS0;
                        bcm2835_gpio_fsel(RPI_V2_GPIO_P1_12, BCM2835_GPIO_FSEL_OUTP);
                bcm2835_gpio_write(RPI_V2_GPIO_P1_12, HIGH);
            break;

        case (SPIDEV_AUX_SPI_CS1):
            sdev->init->device = SPI_DEVICE6_CS0;
                        bcm2835_gpio_fsel(RPI_V2_GPIO_P1_11, BCM2835_GPIO_FSEL_OUTP);
                bcm2835_gpio_write(RPI_V2_GPIO_P1_11, HIGH);

                        // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_12, BCM2835_GPIO_FSEL_INPT);
                        // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_36, BCM2835_GPIO_FSEL_INPT);
            break;

        case (SPIDEV_AUX_SPI_CS2):
            sdev->init->device = SPI_DEVICE6_CS0;
                        bcm2835_gpio_fsel(RPI_V2_GPIO_P1_36, BCM2835_GPIO_FSEL_OUTP);
                bcm2835_gpio_write(RPI_V2_GPIO_P1_36, HIGH);
            break;

        case (SPIDEV_AUX_SPI_CS_NONE):
            sdev->init->device = SPI_DEVICE6_CS0;
            sdev->init->cs_change = 0;
            break;

        default:
            return -1;
    }

    spidev_init(sdev);

    *device = sdev;

    return 0;
}


/* interface function for spidev, this function should be used */
int Aux_Spi_Free(spi_device *device) {
    spidev_free(device);
    bcm2835_close();  // #TODO: may remove this part
    return 0;
}
