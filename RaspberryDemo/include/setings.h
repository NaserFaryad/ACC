#ifndef SETINGS_H
#define SETINGS_H


#include <bcm2835_2.h>
#include <stdint.h>
#include <stdio.h>
#include <linux/spi/spidev.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>


#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

//#define ADC_SPI_RST_PIN     18
#define ADC_SPI_CS_PIN      22
#define ADC_SPI_CS1_PIN     23
#define ADC_SPI_DRDY_PIN    17

#define FunGen_SPI_FSYNC_PIN     7
#define FunGen_SPI_SCLK_PIN      10

#define Sw_sw1_ADC_Dyn_PIN  6
#define Sw_sw2_ADC_Sta_PIN  24
#define Sw_sw3_Fun_Squ_PIN  4
#define Sw_sw4_Fun_Sin_PIN  5

#define AD9833_RESET    0x2100
#define AD9833_Square   0x2028
#define AD9833_Triangle 0x2002
#define AD9833_Sin      0x2000
#define AD9833_Freq0    0x4000
#define AD9833_Freq1    0x8000
#define AD9833_Phase0   0xC000
#define AD9833_Phase1   0xE000


#define BCM2835_SPI5_BASE      0x204A00

extern volatile uint32_t *bcm2835_spi5;
extern uint8_t bcm2835_spi5_transfer(uint8_t value);

typedef enum
{
    ADS1256_GAIN_1			= 0,	/* GAIN   1 */
    ADS1256_GAIN_2			= 1,	/*GAIN   2 */
    ADS1256_GAIN_4			= 2,	/*GAIN   4 */
    ADS1256_GAIN_8			= 3,	/*GAIN   8 */
    ADS1256_GAIN_16			= 4,	/* GAIN  16 */
    ADS1256_GAIN_32			= 5,	/*GAIN    32 */
    ADS1256_GAIN_64			= 6,	/*GAIN    64 */
}ADS1256_GAIN;


typedef enum
{
    ADS1256_BUFF_OFF			= 0,	/* buffer disable */
    ADS1256_BUFF_ON 			= 1,	/* buffer enable */
}ADS1256_Buffer;


typedef enum
{
    ADS1256_30000SPS = 0,
    ADS1256_15000SPS,
    ADS1256_7500SPS,
    ADS1256_3750SPS,
    ADS1256_2000SPS,
    ADS1256_1000SPS,
    ADS1256_500SPS,
    ADS1256_100SPS,
    ADS1256_60SPS,
    ADS1256_50SPS,
    ADS1256_30SPS,
    ADS1256_25SPS,
    ADS1256_15SPS,
    ADS1256_10SPS,
    ADS1256_5SPS,
    ADS1256_2d5SPS,

    ADS1256_DRATE_MAX
}ADS1256_DRATE;


typedef enum
{
    /*Register address, followed by reset the default values */
    REG_STATUS = 0,	// x1H
    REG_MUX    = 1, // 01H
    REG_ADCON  = 2, // 20H
    REG_DRATE  = 3, // F0H
    REG_IO     = 4, // E0H
    REG_OFC0   = 5, // xxH
    REG_OFC1   = 6, // xxH
    REG_OFC2   = 7, // xxH
    REG_FSC0   = 8, // xxH
    REG_FSC1   = 9, // xxH
    REG_FSC2   = 10, // xxH
}ADS1256_REG;


typedef enum
{
    CMD_WAKEUP  = 0x00,	// Completes SYNC and Exits Standby Mode 0000  0000 (00h)
    CMD_RDATA   = 0x01, // Read Data 0000  0001 (01h)
    CMD_RDATAC  = 0x03, // Read Data Continuously 0000   0011 (03h)
    CMD_SDATAC  = 0x0F, // Stop Read Data Continuously 0000   1111 (0Fh)
    CMD_RREG    = 0x10, // Read from REG rrr 0001 rrrr (1xh)
    CMD_WREG    = 0x50, // Write to REG rrr 0101 rrrr (5xh)
    CMD_SELFCAL = 0xF0, // Offset and Gain Self-Calibration 1111    0000 (F0h)
    CMD_SELFOCAL= 0xF1, // Offset Self-Calibration 1111    0001 (F1h)
    CMD_SELFGCAL= 0xF2, // Gain Self-Calibration 1111    0010 (F2h)
    CMD_SYSOCAL = 0xF3, // System Offset Calibration 1111   0011 (F3h)
    CMD_SYSGCAL = 0xF4, // System Gain Calibration 1111    0100 (F4h)
    CMD_SYNC    = 0xFC, // Synchronize the A/D Conversion 1111   1100 (FCh)
    CMD_STANDBY = 0xFD, // Begin Standby Mode 1111   1101 (FDh)
    CMD_RESET   = 0xFE, // Reset to Power-Up Values 1111   1110 (FEh)
}ADS1256_CMD;



static const uint8_t ADS1256_DRATE_E[ADS1256_DRATE_MAX] =
{
    0xF0,		/*reset the default values  */
    0xE0,
    0xD0,
    0xC0,
    0xB0,
    0xA1,
    0x92,
    0x82,
    0x72,
    0x63,
    0x53,
    0x43,
    0x33,
    0x20,
    0x13,
    0x03
};
void ADS1256_Static_config(void);
void ADS1256_Dynamic_config(void);
void ADS1256_initialization(void);
void spi_initialization(void);
void AD9833_initialization(double freq_out);
void AD9833_write(double freq_out,int func_out);
void switch_initialization();
void LTC2992_initialization();

#endif // SETINGS_H
