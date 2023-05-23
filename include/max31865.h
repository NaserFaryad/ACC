/***************************************************************************//**
 *   @file   max31865.h
 *   @brief  Header File of MAX31865 Driver.
 *   @author JSanBuen (jose.sanbuenaventura@analog.com)
 *   @author MSosa (marcpaolo.sosa@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef __MAX31865_H__
#define __MAX31865_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "spidev_lib.h"
#include "bcm2835.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define MAX31865_CS_PIN    RPI_V2_GPIO_P1_36
#define no_os_udelay(d)    bcm2835_delayMicroseconds(d)


#define MAX31865_READ_MASK  			0x7F
#define MAX31865_WRITE_MASK 			0x80

#define MAX31865_CONFIG_BIAS 			0x80
#define MAX31865_CONFIG_MODEAUTO 		0x40
#define MAX31865_CONFIG_MODEOFF 		0xBF
#define MAX31865_CONFIG_1SHOT 			0x20
#define MAX31865_CONFIG_3WIRE 			0x10
#define MAX31865_CONFIG_2_4WIRE 		0xEF
#define MAX31865_CONFIG_CLRFAULT_MASK 	0xD3
#define MAX31865_CONFIG_FAULTSTAT 		0x02
#define MAX31865_CONFIG_FILT50HZ 		0x01
#define MAX31865_CONFIG_FILT60HZ 		0xFE

#define MAX31865_CONFIG_REG 			0x00
#define MAX31865_RTDMSB_REG 			0x01
#define MAX31865_RTDLSB_REG 			0x02
#define MAX31865_HFAULTMSB_REG 			0x03
#define MAX31865_HFAULTLSB_REG 			0x04
#define MAX31865_LFAULTMSB_REG 			0x05
#define MAX31865_LFAULTLSB_REG 			0x06
#define MAX31865_FAULTSTAT_REG 			0x07



typedef enum {
  MAX31865_FAULT_NONE = 0,
  MAX31865_FAULT_AUTO,
  MAX31865_FAULT_MANUAL_RUN,
  MAX31865_FAULT_MANUAL_FINISH
} max31865_fault_cycle_t;


/**
 * @struct max31865_dev
 * @brief Structure holding max31865 descriptor.
 */
struct max31865_dev {
	spi_device *comm_desc;  // Changed
	bool is_filt_50;
	bool is_odd_wire;
	int t_rc_delay;
};

/**
 * @struct max31865_init_param
 * @brief Structure holding the parameters for max31865 initialization.
 */
struct max31865_init_param {
	spi_device *spi_init;  // Changed
	float rtd_rc;
};

/** Device and comm init function */
int max31865_init(struct max31865_dev **, struct max31865_init_param);

/** Free resources allocated by the init function */
int max31865_remove(struct max31865_dev *);

/** Update Register using given update value */
int max31865_reg_update(struct max31865_dev *, uint8_t,uint8_t, bool);

/** Read raw register value */
int max31865_read(struct max31865_dev *, uint8_t, uint8_t *);

/** Write raw register value */
int max31865_write(struct max31865_dev *, uint8_t, uint8_t);

/** Read fault register value */
int max31865_read_fault(struct max31865_dev *, max31865_fault_cycle_t);  //changed

/** Clear all faults */
int max31865_clear_fault(struct max31865_dev *);

/** Enable bias */
int max31865_enable_bias(struct max31865_dev *, bool);

/** Enable auto-convert */
int max31865_auto_convert(struct max31865_dev *, bool);

/** Enable 50Hz filter, default is 60Hz */
int max31865_enable_50Hz(struct max31865_dev *, bool);

/** Set threshold **/
int max31865_set_threshold(struct max31865_dev *, uint16_t, uint16_t);

/** Get Lower threshold **/
int max31865_get_lower_threshold(struct max31865_dev*, uint16_t *);

/** Get Upper threshold **/
int max31865_get_upper_threshold(struct max31865_dev*, uint16_t *);

/** Set Wires **/
int max31865_set_wires(struct max31865_dev *, bool);

/** Read RTD **/
int max31865_read_rtd(struct max31865_dev *, uint16_t *);


/* Extra */

/* Find first set bit in word. */
uint32_t no_os_find_first_set_bit(uint32_t word);

/* Get a field specified by a mask from a word. */
uint32_t no_os_field_get(uint32_t mask, uint32_t word);




#define MAX31865_FAULT_HIGHTHRESH 0x80
#define MAX31865_FAULT_LOWTHRESH 0x40
#define MAX31865_FAULT_REFINLOW 0x20
#define MAX31865_FAULT_REFINHIGH 0x10
#define MAX31865_FAULT_RTDINLOW 0x08
#define MAX31865_FAULT_OVUV 0x04

#endif // __MAX31865_H__