#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "bcm2835.h"
#include "ad717x.h"
#include <stdbool.h>
//typedef int bool;
//#define true 1
//#define false 0

/* Error Codes*/
#define BCM2835_INIT_ERROR          -1
#define BCM2835_SPI_BEGIN_ERROR     -2
#define AD717X_INIT_ERROR           -3
#define AD7175_ChipID_ERROR         -4 //Can not verify AD7175's id
#define AD717X_COMM_ERROR           -5 /* Communication error on receive */
#define AD717X_TIMEOUT_ERROR        -6 /* A timeout has occured */
#define AD717X_SETCHNL_ERROR        -7 /* A set channel has occured */
#define AD717x_CON_ANALOG_IN_ERROR  -8 /* A Set Analog Inputs to channel error has occurred*/
#define AD717x_SET_POLARITY_ERROR   -9 /* A Set Polarity error has occurred*/
#define AD717x_SET_REF_SRC_ERROR    -10/* When Selecting the reference source, an error has occurred.*/
#define AD717x_ENABLE_IN_BUF_ERROR  -11/* A Enable Input Buffer error has occurred.*/
#define AD717x_CONF_DEV_ODR_ERROR   -12/* An error has occured when Configure ODR for the device.*/
#define AD717x_ASSIGN_SETUP_ERROR   -13/* Assign Setup to Channel error has occurred.*/
#define AD717x_SINGLE_READ_ERROR    -14/*Perform Single Conversion error has occurred.*/
#define AD717x_SET_ADC_MODE_ERROR   -15/*Set ADC Mode error has occured.*/
#define AD717X_SET_FILTR_TYPE_ERROR -16/*Configure Filter for the device error has occured.*/
#define AD717X_WAITING_ERROR        -17/*Waits until a new conversion result is available error*/
#define AD717X_READ_DATA_ERROR      -18/*Reads the conversion result from the device error*/











#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))


#define INITIALIZE_MAX_RETRY 3

#define RELAY_K1_GPIO 6
#define RELAY_K2_GPIO 22
#define RELAY_K3_GPIO 14


/* AD7175 */
#define AD7175_2_CHIP_ID 0xcd
#define CHANNEL_ID_0 0
#define CHANNEL_ID_1 1
#define CHANNEL_ID_2 2
#define CHANNEL_ID_3 3

#define SETUP_ID_0 0
#define SETUP_ID_1 1
#define SETUP_ID_2 2
#define SETUP_ID_3 3

#define FILTER_ID_0 0
#define FILTER_ID_1 1
#define FILTER_ID_2 2
#define FILTER_ID_3 3

#define GAIN_ID_0 0
#define GAIN_ID_1 1
#define GAIN_ID_2 2
#define GAIN_ID_3 3

/* AD9833 */
#define AD9833_OUT_SINUSOID 0
#define AD9833_OUT_TRIANGLE 1
#define AD9833_OUT_DAC_DATA_MSBDIV2 2
#define AD9833_OUT_DAC_DATA_MSB 3




int bcm2835_and_spi_init();
int spi_free();
bool AD7175_VerifyChipID(ad717x_dev **dev);
void WriteRegister(ad717x_dev *dev, uint8_t addr, int32_t val);
int Initialize_Device(ad717x_dev **device, ad717x_init_param init_param, int max_retry);
int Static_Init(ad717x_dev **device);
int Static_Read(ad717x_dev *dev, int32_t *adc_data);
int Dynamic_Init(ad717x_dev **device);
int Dynamic_Read(ad717x_dev *dev, int32_t *adc_data);


#ifdef __cplusplus
}
#endif

#endif // TEST_H
