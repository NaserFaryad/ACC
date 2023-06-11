/*
 * AD7705/AD7706 Library
 * Kerry D. Wong
 * http://www.kerrywong.com
 * Initial version 1.0 3/2011
 * Updated 1.1 4/2012
 */

#include "ad770x.h"


int AD7706_SPI_Init() {
    if (!bcm2835_init())
    {
        printf("bcm2835_init failed. Are you running as root??\n");
        return -1;
    }

    if (!bcm2835_aux_spi_begin())
    {
        printf("bcm2835_spi_begin failed. Are you running as root??\n");
        return -1;
    }

    bcm2835_gpio_fsel(AD7706_CS, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(AD7706_CS, HIGH);
//    GPIOExport(AD7706_CS);
//    GPIODirection(AD7706_CS, OUT);
//    GPIOWrite(AD7706_CS, HIGH);
    return 0;
}

uint8_t spiTransfer(spi_device *spi_dev, volatile uint8_t data) {
    uint8_t value;
    uint8_t txBuf[1] = {0x0};
    uint8_t rxBuf[1] = {0x0};

    txBuf[0] = data;
    // bcm2835_aux_spi_transfern(buf, 1);
    // spidev_transfernb(spi_dev, buf, 1);

    spidev_transfer(spi_dev, txBuf, rxBuf, 1);

    value = rxBuf[0];
    return value;
};



//write communication register
//   7        6      5      4      3      2      1      0
//0/DRDY(0) RS2(0) RS1(0) RS0(0) R/W(0) STBY(0) CH1(0) CH0(0)

void setNextOperation(spi_device *spi_dev, uint8_t reg, uint8_t channel, uint8_t readWrite) {
    uint8_t r = 0;
    r = reg << 4 | readWrite << 3 | channel;

    // bcm2835_gpio_write(AD7706_CS, LOW);
    spiTransfer(spi_dev, r);
    // bcm2835_gpio_write(AD7706_CS, HIGH);
}

//Clock Register
//   7      6       5        4        3        2      1      0
//ZERO(0) ZERO(0) ZERO(0) CLKDIS(0) CLKDIV(0) CLK(1) FS1(0) FS0(1)
//
//CLKDIS: master clock disable bit
//CLKDIV: clock divider bit

void writeClockRegister(spi_device *spi_dev, uint8_t CLKDIS, uint8_t CLKDIV, uint8_t outputUpdateRate) {
    uint8_t r = CLKDIS << 4 | CLKDIV << 3 | outputUpdateRate;

    r &= ~(1 << 2); // clear CLK

    // bcm2835_gpio_write(AD7706_CS, LOW);
    spiTransfer(spi_dev, r);
    // bcm2835_gpio_write(AD7706_CS, HIGH);
}

//Setup Register
//  7     6     5     4     3      2      1      0
//MD10) MD0(0) G2(0) G1(0) G0(0) B/U(0) BUF(0) FSYNC(1)

void writeSetupRegister(spi_device *spi_dev, uint8_t operationMode, uint8_t gain, uint8_t unipolar, uint8_t buffered, uint8_t fsync) {
    uint8_t r = operationMode << 6 | gain << 3 | unipolar << 2 | buffered << 1 | fsync;

    // bcm2835_gpio_write(AD7706_CS, LOW);
    spiTransfer(spi_dev, r);
    // bcm2835_gpio_write(AD7706_CS, HIGH);
}

unsigned int readADResultUnsignedInt(spi_device *spi_dev) {
    // bcm2835_gpio_write(AD7706_CS, LOW);
    uint8_t b1 = spiTransfer(spi_dev, 0x0);
    uint8_t b2 = spiTransfer(spi_dev, 0x0);
    // bcm2835_gpio_write(AD7706_CS, HIGH);

    unsigned int r = b1 << 8 | b2;

    return r;
}

int readADResultDouble(spi_device *spi_dev, double *result,uint8_t channel, float refOffset, double VRef, uint8_t polarity, int Gain) {
    double result_local = 0.0f;
    unsigned int ad_result_uint = 0;
    // float refOffset = 0.0;
    int ret = 0;
    ret = dataReady(spi_dev, channel, 1000000);
    if (ret < 0)
    {
        return ret;
    }
    bcm2835_gpio_write(AD7706_CS, LOW);
    setNextOperation(spi_dev, REG_DATA, channel, 1);
    ad_result_uint = readADResultUnsignedInt(spi_dev);
    bcm2835_gpio_write(AD7706_CS, HIGH);

    // printf("Channel: %d - Raw Value=%d\n", channel, ad_result_uint);
    // int GAIN = 1;

    if (polarity == UNIPOLAR) {  // Unipolar
        result_local = (double) (VRef / (double)(65536 - 1)) * ((double)ad_result_uint / (double)Gain);
    } else if (polarity == BIPOLAR) {  // bipolar
        result_local = (double) (VRef / (double)(32768 - 1)) * ((double)ad_result_uint / Gain);
    } else {
        return -2;
    }
    // result = (double) ((double)ad_result_uint-32768) * 1.0 / 32768.0 * VRef - refOffset;
    // result = (double) (VRef / (65535)) * ((double)ad_result_uint / GAIN);  //65535
    *result = result_local;
    return ret;
}

int32_t dataReady(spi_device *spi_dev, uint8_t channel, int32_t timeout) {
    uint8_t b1 = 0x0;

    uint8_t ready = 0;
//    uint32_t timeout = 500;
    int TIMEOUT = -3;

    uint8_t pin_value;

     while(!ready && --timeout) {
            ready = bcm2835_gpio_lev(RPI_V2_GPIO_P1_13);
         }

     return timeout ? 0 : TIMEOUT;
//    ready = bcm2835_gpio_lev(RPI_V2_GPIO_P1_13);
//    ready = GPIORead(RPI_V2_GPIO_P1_13);

//    return ! ready;

    // bcm2835_gpio_write(AD7706_CS, LOW);
    // setNextOperation(spi_dev, REG_CMM, channel, 1);
    // b1 = spiTransfer(spi_dev, 0x0);
    // bcm2835_gpio_write(AD7706_CS, HIGH);

    // return (b1 & 0x80) == 0x0;
}

void ad7706Reset(spi_device *spi_dev) {
//    GPIOWrite(AD7706_CS, LOW);
    bcm2835_gpio_write(AD7706_CS, LOW);
    for (int i = 0; i < 100; i++)
        spiTransfer(spi_dev, 0xff);
//    GPIOWrite(AD7706_CS, HIGH);
    bcm2835_gpio_write(AD7706_CS, HIGH);
}


void ad7706Init(spi_device *spi_dev, uint8_t channel, uint8_t clkDivider, uint8_t polarity, uint8_t gain, uint8_t updRate) {
    bcm2835_gpio_fsel(RPI_V2_GPIO_P1_13, BCM2835_GPIO_FSEL_INPT);  // set pin 13 to Input
//    GPIOExport(RPI_V2_GPIO_P1_13);
//    GPIODirection(RPI_V2_GPIO_P1_13,IN);


//    GPIOWrite(AD7706_CS, LOW);
    bcm2835_gpio_write(AD7706_CS, LOW);
    setNextOperation(spi_dev, REG_CLOCK, channel, 0);
    writeClockRegister(spi_dev, 0, clkDivider, updRate);
//    GPIOWrite(AD7706_CS, HIGH);
    bcm2835_gpio_write(AD7706_CS, HIGH);

//    GPIOWrite(AD7706_CS, LOW);
    bcm2835_gpio_write(AD7706_CS, LOW);
    setNextOperation(spi_dev, REG_SETUP, channel, 0);
    writeSetupRegister(spi_dev, MODE_SELF_CAL, gain, polarity, 0, 0);
//    GPIOWrite(AD7706_CS, HIGH);
    bcm2835_gpio_write(AD7706_CS, HIGH);

    int ret = dataReady(spi_dev, channel, 100000);
    if (ret < 0)
    {
        fprintf(stderr, "!!!!!!!!!! Time out Error!!!!!!!!!!!!!!");
    }
    /*while (!dataReady(spi_dev, channel)) {
    };*/
}

void ad7706InitDefaultParams(spi_device *spi_dev, uint8_t channel) {
    ad7706Init(spi_dev, channel, CLK_DIV_1, BIPOLAR, GAIN_1, UPDATE_RATE_25);
}
