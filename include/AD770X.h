#ifndef AD770X_H
#define AD770X_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <bcm2835.h>
#include "spidev_lib.h"

/*
 * AD7705/AD7706 Library
 * Kerry D. Wong
 * http://www.kerrywong.com
 * Initial version 1.0 3/2011
 * Updated 1.1 4/2012
 */


#define AD7706_MOSI     RPI_V2_GPIO_P1_38
#define AD7706_MISO     RPI_V2_GPIO_P1_35
#define AD7706_CLK      RPI_V2_GPIO_P1_40
#define AD7706_CS       RPI_V2_GPIO_P1_11


//register selection 
//RS2 RS1 RS0
#define  REG_CMM     0x0 //communication register 8 bit
#define  REG_SETUP   0x1 //setup register 8 bit
#define  REG_CLOCK   0x2 //clock register 8 bit
#define  REG_DATA    0x3 //data register 16 bit, contains conversion result
#define  REG_TEST    0x4 //test register 8 bit, POR 0x0
#define  REG_NOP     0x5 //no operation
#define  REG_OFFSET  0x6 //offset register 24 bit
#define  REG_GAIN    0x7 // gain register 24 bit

//channel selection for AD7706 (for AD7705 use the first two channel definitions)
//CH1 CH0
#define  CHN_AIN1  0x0 //AIN1; calibration register pair 0
#define  CHN_AIN2  0x1 //AIN2; calibration register pair 1
#define  CHN_COMM  0x2 //common; calibration register pair 0
#define  CHN_AIN3  0x3 //AIN3; calibration register pair 2

//output update rate
//CLK FS1 FS0
#define  UPDATE_RATE_20   0x0 // 20 Hz
#define  UPDATE_RATE_25   0x1 // 25 Hz
#define  UPDATE_RATE_100  0x2 // 100 Hz
#define  UPDATE_RATE_200  0x3 // 200 Hz
#define  UPDATE_RATE_50   0x4 // 50 Hz
#define  UPDATE_RATE_60   0x5 // 60 Hz
#define  UPDATE_RATE_250  0x6 // 250 Hz
#define  UPDATE_RATE_500  0x7 // 500 Hz

//operating mode options
//MD1 MD0
#define  MODE_NORMAL          0x0 //normal mode
#define  MODE_SELF_CAL        0x1 //self-calibration
#define  MODE_ZERO_SCALE_CAL  0x2 //zero-scale system calibration, POR 0x1F4000, set FSYNC high before calibration, FSYNC low after calibration
#define  MODE_FULL_SCALE_CAL  0x3 //full-scale system calibration, POR 0x5761AB, set FSYNC high before calibration, FSYNC low after calibration

//gain setting
#define  GAIN_1    0x0
#define  GAIN_2    0x1
#define  GAIN_4    0x2
#define  GAIN_8    0x3
#define  GAIN_16   0x4
#define  GAIN_32   0x5
#define  GAIN_64   0x6
#define  GAIN_128  0x7

#define  UNIPOLAR  0x0
#define  BIPOLAR   0x1

#define  CLK_DIV_1  0x0
#define  CLK_DIV_2  0x1


int AD7706_SPI_Init();
uint8_t spiTransfer(spi_device *spi_dev, volatile uint8_t data);


void setNextOperation(spi_device *spi_dev, uint8_t reg, uint8_t channel, uint8_t readWrite);
void writeClockRegister(spi_device *spi_dev, uint8_t CLKDIS, uint8_t CLKDIV, uint8_t outputUpdateRate);
void writeSetupRegister(spi_device *spi_dev, uint8_t operationMode, uint8_t gain, uint8_t unipolar, uint8_t buffered, uint8_t fsync);
double readADResultDouble(spi_device *spi_dev, uint8_t channel, float refOffset, double VRef, uint8_t polarity, int Gain);
void ad7706Reset(spi_device *spi_dev);
bool dataReady(spi_device *spi_dev, uint8_t channel);
void ad7706InitDefaultParams(spi_device *spi_dev, uint8_t channel);
void ad7706Init(spi_device *spi_dev, uint8_t channel, uint8_t clkDivider, uint8_t polarity, uint8_t gain, uint8_t updRate);

unsigned int readADResultUnsignedInt();

#endif
