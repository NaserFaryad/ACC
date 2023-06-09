#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include "DEV_Config.h"
#include "ad7175_2_regs.h"
#include "ad9833.h"
#include "AD770X.h"
#include "max31865.h"
#include "spidev_lib.h"



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


int bcm2835_and_spi_init()
{
    if (!bcm2835_init())
    {
        printf("bcm2835_init failed. Are you running as root??\n");
        return -1;
    }

    if (!bcm2835_spi_begin())
    {
        printf("bcm2835_spi_begin failed. Are you running as root??\n");
        return -1;
    }

    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128); 
    // bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    // bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
    bcm2835_gpio_fsel(AD7175_2_CS_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(AD7175_2_CS_PIN, HIGH);
    return 0;
}

int spi_free() {
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}

bool AD7175_VerifyChipID(ad717x_dev **dev) {
    AD717X_ReadRegister(*dev, AD717X_ID_REG);
    ad717x_st_reg *pReg;
    pReg = AD717X_GetReg(*dev, AD717X_ID_REG);
	if (!pReg){
        printf("error getting register\n");
		return false;
    }
    
    // AD7175-2 ID ===> 0X0CDX, 4 LSB IS DON'T CARE. SO WE SHIFT THE VALUE BY 4 TO IGNORE IT
    if(pReg->value >> 4 == (int32_t)AD7175_2_CHIP_ID) {
        return true;
    }
    return false;
}

void PrintData(ad717x_dev *dev, uint8_t addr) {
    AD717X_ReadRegister(dev, addr);
    ad717x_st_reg *pReg;
    pReg = AD717X_GetReg(dev, addr);
	if (!pReg){
        printf("error getting register\n");
		return;
    }

    uint8_t val[pReg->size];
    memset(val, 0, pReg->size);

    int32_t tempVal = pReg->value;
    for(int i=(pReg->size-1); i>=0; i--) {
        val[i] = tempVal;
        tempVal >>= 8;
    }

    printf("Value of Register Adddres: %02X is: ", pReg->addr);
    for(int i=0; i<pReg->size; i++) {
        printf("%02X ", val[i]);
    }
    printf("\n");

}

void test(uint8_t addr) {
    // uint8_t write[3] = {0x02, 0x0, 0x0};  // write
    uint8_t write[3] = {0x42, 0x0, 0x0};  //read
    uint8_t read[3] = {0x0, 0x0, 0x0};
    bcm2835_spi_transfernb(write, read, 3);

    for(int i=1; i<3; i++) {
        printf("%02X ", read[i]);
    }
    printf("\n");
}

void WriteRegister(ad717x_dev *dev, uint8_t addr, int32_t val) {
    ad717x_st_reg *pReg;
    pReg = AD717X_GetReg(dev, addr);
	if (!pReg){
        printf("error getting register\n");
		return;
    }
    pReg->value = val;
    AD717X_WriteRegister(dev, addr);
}



// void WriteRegisterDirectlyWithSpi(uint8_t addr, int32_t  val) {
//     uint8_t crc8     = 0;
//     uint8_t write[4] = {0x02, 0x00, 0x0, 0x0};  // write
//     uint8_t read[4] = {0x0, 0x0, 0x0, 0x0};
//     /* Compute the CRC */
// 	if(1) {
// 		crc8 = AD717X_ComputeCRC8(write, 2 + 1);
// 		write[2 + 1] = crc8;
// 	}
//     // bcm2835_spi_transfernb(write, read, 4);
//     bcm2835_spi_writenb(write, 4);
// }

int Initialize_Device(ad717x_dev **device, ad717x_init_param init_param, int max_retry) {
    int ret = 0;
    bool success_flag = false;
    uint8_t counter = 0;

    while(counter <= max_retry){
        ret = AD717X_Init(device, init_param);
        if (ret < 0) {
            perror("error initialize\n");
            ++counter;
        } else {
            success_flag = true;
            break;  
        }    
    }

    if (!success_flag)
        return -10;

    counter = 0;
    success_flag = false;
    while(counter <= max_retry){
        if(!AD7175_VerifyChipID(device)){
            perror("Can not verify AD7175's id\n");
            ++counter;
        } else {
            success_flag = true;
            break;  
        }
    }

    if (!success_flag)
        return -11;
    
    return 0;
}


int Static_Init(ad717x_dev **device) {
    int ret = 0;
    /* Initialize ADC */
    ad717x_dev *dev;
    ad717x_init_param ad7175_2_init;
    int num_regs = ARRAY_SIZE(ad7175_2_regs);
    ad7175_2_init.num_regs = num_regs;

    ad7175_2_init.regs = malloc(sizeof(ad717x_st_reg) * num_regs);
    memcpy(ad7175_2_init.regs, &ad7175_2_regs, sizeof(ad7175_2_regs));

    ad7175_2_init.active_device = ID_AD7175_2;
    ad7175_2_init.ref_en = true;
    ad7175_2_init.num_channels = 1;
    ad7175_2_init.num_setups = 1;
    ad7175_2_init.mode = SINGLE;

    struct ad717x_filtcon filtr = {
        .enhfilten = false,
        .oder = sinc3,
        .odr = sps_503
    };
    ad7175_2_init.filter_configuration[0] = filtr;

    /* Initialize bcm2835 with SPI */
    ret = bcm2835_and_spi_init();
    if (ret == -1) {
        printf("bcm2835_init or bcm2835_spi_begin failed.\n");
        return -1;
    }

    /* Set Relay off */
    /* RELAY K1 */
    bcm2835_gpio_fsel(RELAY_K1_GPIO, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY_K1_GPIO, LOW);
    bcm2835_delay(1);

    /* RELAY K2 */
    bcm2835_gpio_fsel(RELAY_K2_GPIO, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY_K2_GPIO, LOW);
    bcm2835_delay(1);

    /* RELAY K3 */
    bcm2835_gpio_fsel(RELAY_K3_GPIO, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY_K3_GPIO, LOW);
    bcm2835_delay(1);

    /* Initialize AD7175-2 ADC */
    ret = Initialize_Device(&dev, ad7175_2_init, INITIALIZE_MAX_RETRY);
    if (ret < 0) return ret;

    /* Registers setup */

    WriteRegister(dev, AD717X_IFMODE_REG, 0x108);  // Interface Register

    ret = ad717x_set_channel_status(dev, CHANNEL_ID_0, true);  // enable channel 1
    if (ret < 0) return ret;
    ret = ad717x_set_channel_status(dev, CHANNEL_ID_1, false);  // disable channel 2
    if (ret < 0) return ret;
    ret = ad717x_set_channel_status(dev, CHANNEL_ID_2, false);  // disable channel 3
    if (ret < 0) return ret;
    ret = ad717x_set_channel_status(dev, CHANNEL_ID_3, false);  // disable channel 4
    if (ret < 0) return ret;

    /*Important: for AD7175-2 only "ainp" srtuct should be assigned, no need to "ad717x_analog_input_pairs" */
    union ad717x_analog_inputs channel0Input;
    channel0Input.ainp.pos_analog_input = AIN0;
    channel0Input.ainp.neg_analog_input = AIN4;
    ret = ad717x_connect_analog_input(dev, CHANNEL_ID_0, channel0Input);
    if (ret < 0) return ret;

    ret = ad717x_set_polarity(dev, true, SETUP_ID_0);  // bipolar or unipolar -> true: bipolar, false: unipolar
    if (ret < 0) return ret;

    ret = ad717x_set_reference_source(dev, INTERNAL_REF, SETUP_ID_0);  // set INTERNAL_REF for setup 0
    if (ret < 0) return ret;

    ret = ad717x_enable_input_buffer(dev, true, true, SETUP_ID_0);  // enable input and reference source
    if (ret < 0) return ret;

    ret = ad717x_configure_device_odr(dev, FILTER_ID_0, sps_503);  // Set sample rate
    if (ret < 0) return ret;

    ret = ad717x_assign_setup(dev, CHANNEL_ID_0, SETUP_ID_0);  // Assign setup 0 to channel 0
    if (ret < 0) return ret;

    WriteRegister(dev, AD717X_ADCMODE_REG, 0xA010);


    WriteRegister(dev, AD717X_FILTCON0_REG, 0x572);

    // WriteRegister(dev, AD717X_SETUPCON0_REG, 0x1F20); // TEMP

    *device = dev;

    return ret;
}

int Static_Read(ad717x_dev *dev, int32_t *adc_data) {
    int ret = 0;
    int32_t adc_data_temp = 0;
    int32_t adc_data_sum = 0;
    int counter = 0;
    while(counter < 5)
    {
        ret = ad717x_single_read(dev, CHANNEL_ID_0, &adc_data_temp);
        if (ret < 0) return ret;
        adc_data_sum += adc_data_temp;
        counter++;
    }
    *adc_data = adc_data_sum / counter;
    return 0;
}

int Dynamic_Init(ad717x_dev **device) {
    int ret = 0;
    /* Initialize ADC */
    ad717x_dev *dev;
    ad717x_init_param ad7175_2_init;
    int num_regs = ARRAY_SIZE(ad7175_2_regs);
    ad7175_2_init.num_regs = num_regs;

    ad7175_2_init.regs = malloc(sizeof(ad717x_st_reg) * num_regs);
    memcpy(ad7175_2_init.regs, &ad7175_2_regs, sizeof(ad7175_2_regs));

    ad7175_2_init.active_device = ID_AD7175_2;
    ad7175_2_init.ref_en = true;
    ad7175_2_init.num_channels = 1;
    ad7175_2_init.num_setups = 1;
    ad7175_2_init.mode = CONTINUOUS;

    struct ad717x_filtcon filtr = {
        .enhfilten = false,
        .oder = sinc3,
        .odr = sps_1007
    };
    ad7175_2_init.filter_configuration[0] = filtr;

    ret = bcm2835_and_spi_init();
    if (ret == -1) {
        printf("bcm2835_init or bcm2835_spi_begin failed.\n");
        return -1;
    }

    ret = Initialize_Device(&dev, ad7175_2_init, INITIALIZE_MAX_RETRY);
    if (ret < 0) return ret;

    
    /* Registers setup */

    WriteRegister(dev, AD717X_IFMODE_REG, 0x108);  // Interface Register

    ret = ad717x_set_channel_status(dev, CHANNEL_ID_0, true);  // enable channel 1
    if (ret < 0) return ret;
    ret = ad717x_set_channel_status(dev, CHANNEL_ID_1, false);  // disable channel 2
    if (ret < 0) return ret;
    ret = ad717x_set_channel_status(dev, CHANNEL_ID_2, false);  // disable channel 3
    if (ret < 0) return ret;
    ret = ad717x_set_channel_status(dev, CHANNEL_ID_3, false);  // disable channel 4
    if (ret < 0) return ret;

    /*Important: for AD7175-2 only "ainp" srtuct should be assigned, no need to "ad717x_analog_input_pairs" */
    union ad717x_analog_inputs channel0Input;
    channel0Input.ainp.pos_analog_input = AIN0;
    // channel0Input.ainp.pos_analog_input = AIN2; <-- this is final
    channel0Input.ainp.neg_analog_input = AIN4;
    ret = ad717x_connect_analog_input(dev, CHANNEL_ID_0, channel0Input);
    if (ret < 0) return ret;

    
    ret = ad717x_set_polarity(dev, false, SETUP_ID_0);  // bipolar or unipolar -> true: bipolar, false: unipolar
    if (ret < 0) return ret;

    ret = ad717x_set_reference_source(dev, INTERNAL_REF, SETUP_ID_0);  // set INTERNAL_REF for setup 0
    if (ret < 0) return ret;

    ret = ad717x_enable_input_buffer(dev, true, true, SETUP_ID_0);  // enable input and reference source
    if (ret < 0) return ret;

    ret = ad717x_configure_device_odr(dev, FILTER_ID_0, sps31250_e);  // Set sample rate
    if (ret < 0) return ret;

    ret = ad717x_assign_setup(dev, CHANNEL_ID_0, SETUP_ID_0);  // Assign setup 0 to channel 0
    if (ret < 0) return ret;

    /* Set Mode to CONTINUOUS Conversion */
	ret = ad717x_set_adc_mode(dev, CONTINUOUS);
	if (ret < 0) return ret;

    WriteRegister(dev, AD717X_FILTCON0_REG, 0x567);  // Filter 0 Register
    // PrintData(dev, AD717X_FILTCON0_REG);

    WriteRegister(dev, AD717X_ADCMODE_REG, 0xA000);  // ADC Mode Register

    // WriteRegister(dev, AD717X_IFMODE_REG, 0x148);  // Interface Register
    // PrintData(dev, AD717X_IFMODE_REG);

    *device = dev;
    
    return ret;

}

int Dynamic_Read(ad717x_dev *dev, int32_t adc_data) {
    int ret = 0;
    ret = AD717X_ReadData(dev, &adc_data);
    if (ret < 0) return ret;
}




int Sinusoid_Init(struct ad9833_dev **device) {
    int ret = 0;
    spi_device *spi_dev;
    struct ad9833_dev *dev;
    struct ad9833_init_param init_param;

    ret = Aux_Spi_Init(&spi_dev, 50000, SPI_MODE_2, 1, SPIDEV_AUX_SPI_CS0);
    if (ret < 0) {
        printf("Error initilizing.......");
        return -1000;
    }

    /* Set Relay ON */
    /* RELAY K1 */
    bcm2835_gpio_fsel(RELAY_K1_GPIO, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY_K1_GPIO, HIGH);  // in Square wave mode, the relay shoul be oOn
    bcm2835_delay(1);

    init_param.act_device = ID_AD9833;
    init_param.spi_dev = spi_dev;

    ret = ad9833_init(&dev, init_param);
    if (ret < -1) {  // TODO: FIX ERROR CODE
        printf("Error initializing ad9833.\n");
        return -19393;
    }

    ad9833_out_mode(dev, AD9833_OUT_SINUSOID);  // #TODO:

    *device = dev;

    return 0;
}

int Sinusoid_Gen(struct ad9833_dev *dev) {
    ad9833_set_freq(dev, 0, 1000);
    ad9833_set_phase(dev, 0, 0);
    ad9833_select_freq_reg(dev, 0);
    ad9833_select_phase_reg(dev, 0);
}

int Square_Init(struct ad9833_dev **device) {
    int ret = 0;
    spi_device *spi_dev;
    struct ad9833_dev *dev;
    struct ad9833_init_param init_param;

    ret = Aux_Spi_Init(&spi_dev, 50000, SPI_MODE_2, 1, SPIDEV_AUX_SPI_CS0);
    if (ret < 0) {
        printf("Error initilizing.......");
        return -1000;
    }

    /* Set Relay OFF */
    /* RELAY K1 */
    bcm2835_gpio_fsel(RELAY_K1_GPIO, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY_K1_GPIO, LOW);  // in Square wave mode, the relay shoul be off
    bcm2835_delay(1);

    init_param.act_device = ID_AD9833;
    init_param.spi_dev = spi_dev;

    ret = ad9833_init(&dev, init_param);
    if (ret < -1) {  // TODO: FIX ERROR CODE
        printf("Error initializing ad9833.\n");
        return -19393;
    }

    ad9833_out_mode(dev, AD9833_OUT_DAC_DATA_MSB);

    *device = dev;

    return 0;
}

int Square_Gen(struct ad9833_dev *dev) {
    ad9833_set_freq(dev, 0, 5000.0);
    ad9833_set_phase(dev, 0, 0.0);
    ad9833_select_freq_reg(dev, 0);
    ad9833_select_phase_reg(dev, 0);

}

int AD7706_Init() {
    int ret = 0;
    spi_device *spi_dev;
    double sum_ch1 = 0;
    double sum_ch2 = 0;
    double avg_ch1 = 0;
    double avg_ch2 = 0;
    double current_P15 = 0;
    double current_N15 = 0;

    Aux_Spi_Init(&spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS_NONE);
    ad7706Reset(spi_dev);
    ad7706Init(spi_dev, CHN_AIN1, CLK_DIV_1, UNIPOLAR, GAIN_1, UPDATE_RATE_50);
    ad7706Init(spi_dev, CHN_AIN2, CLK_DIV_1, UNIPOLAR, GAIN_1, UPDATE_RATE_50);
    
    int i = 0;
    while (1) {
        sum_ch1 = 0;
        sum_ch2 = 0;
        double start_time = clock();
        for(i = 0; i< 12; i++){
            sum_ch1 += readADResultDouble(spi_dev, CHN_AIN1, 0, 1.225, UNIPOLAR, 1);
        }
        for(i = 0; i< 12; i++){
            sum_ch2 += readADResultDouble(spi_dev, CHN_AIN2, 0, 1.225, UNIPOLAR, 1);
        }
        double stop_time = clock();
        avg_ch1 = sum_ch1 / i;
        avg_ch2 = sum_ch2 / i;
        current_P15 = (avg_ch1 + 0) / 5.6;
        current_N15 = (avg_ch2 + 0) / 5.6;
        printf("channel 1 = %f  \n", current_P15);
        printf("channel 2 = %f  \n", current_N15);
        // printf("time = %f\n",(stop_time - start_time)/CLOCKS_PER_SEC);
        // bcm2835_delay(100);
    }
    
    Aux_Spi_Free(spi_dev);

}


// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      4120.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  1000.0

#define RTD_A 3.9083e-3
#define RTD_B -5.775e-7



/**************************************************************************/
/*!
    @brief Calculate the temperature in C from the RTD through calculation of
   the resistance. Uses
   http://www.analog.com/media/en/technical-documentation/application-notes/AN709_0.pdf
   technique
    @param RTDraw The raw 16-bit value from the RTD_REG
    @param RTDnominal The 'nominal' resistance of the RTD sensor, usually 100
    or 1000
    @param refResistor The value of the matching reference resistor, usually
    430 or 4300
    @returns Temperature in C
*/
/**************************************************************************/
float calculateTemperature(uint16_t RTDraw, float RTDnominal, float refResistor) {
  float Z1, Z2, Z3, Z4, Rt, temp;

  Rt = RTDraw;
  Rt /= 32768;
  Rt *= refResistor;

  // Serial.print("\nResistance: "); Serial.println(Rt, 8);

  Z1 = -RTD_A;
  Z2 = RTD_A * RTD_A - (4 * RTD_B);
  Z3 = (4 * RTD_B) / RTDnominal;
  Z4 = 2 * RTD_B;

  temp = Z2 + (Z3 * Rt);
  temp = (sqrt(temp) + Z1) / Z4;

  if (temp >= 0)
    return temp;

  // ugh.
  Rt /= RTDnominal;
  Rt *= 100; // normalize to 100 ohm

  float rpoly = Rt;

  temp = -242.02;
  temp += 2.2228 * rpoly;
  rpoly *= Rt; // square
  temp += 2.5859e-3 * rpoly;
  rpoly *= Rt; // ^3
  temp -= 4.8260e-6 * rpoly;
  rpoly *= Rt; // ^4
  temp -= 2.8183e-8 * rpoly;
  rpoly *= Rt; // ^5
  temp += 1.5243e-10 * rpoly;

  return temp;
}

int max31865_Init() {
    int ret = 0;
    spi_device *spi_dev;

    struct max31865_dev *max31865_dev;
    struct max31865_init_param init_param;

    ret = Aux_Spi_Init(&spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS2);
    if (ret<0) return ret;

    init_param.spi_init = spi_dev;
    init_param.rtd_rc = 0.0095f;  // TODO

    


    ret = max31865_init(&max31865_dev, init_param);
    if (ret<0) return ret;

    

    ret = max31865_set_wires(max31865_dev, false); // true: 3-wires, false: 4-wires

    max31865_enable_bias(max31865_dev, false);
    max31865_auto_convert(max31865_dev, true);

    max31865_set_threshold(max31865_dev, 0x0, (uint16_t)0xFFFF);

    // max31865_enable_50Hz(max31865_dev, true);  // true: 50Hz, false: 60Hz
    // 

    max31865_clear_fault(max31865_dev);

    // bcm2835_gpio_fsel(RPI_V2_GPIO_P1_32, BCM2835_GPIO_FSEL_INPT);  // set pin 32 to INPUT

    uint8_t ready;

    // while(bcm2835_gpio_lev(RPI_V2_GPIO_P1_13)){};
    uint16_t raw_val = 0;
    uint8_t fault;

    float ratio = 0.00f;
    int i = 0;
    while (1) {
        ret = max31865_read_rtd(max31865_dev, &raw_val);
        if(ret<0) return ret;
        // printf("raw value= %d\n", raw_val);
        ratio = (float) raw_val / 32768.0f;
        printf("Resistor= %f\n", RREF * ratio);
        // float temp = ((float)raw_val / 32768.0) * 425.0;
        // printf("%f\n", temp);
        printf("Tempreature= %f\n", calculateTemperature(raw_val, RNOMINAL, RREF));

        fault = max31865_read_fault(max31865_dev, MAX31865_FAULT_AUTO);
        if (fault) {
            printf("Fault 0x"); 
            printf("%02X\n", fault);
            
            if (fault & MAX31865_FAULT_HIGHTHRESH) {
                printf("RTD High Threshold\n"); 
            }
            if (fault & MAX31865_FAULT_LOWTHRESH) {
                printf("RTD Low Threshold\n"); 
            }
            if (fault & MAX31865_FAULT_REFINLOW) {
                printf("REFIN- > 0.85 x Bias\n"); 
            }
            if (fault & MAX31865_FAULT_REFINHIGH) {
                printf("REFIN- < 0.85 x Bias - FORCE- open\n"); 
            }
            if (fault & MAX31865_FAULT_RTDINLOW) {
                printf("RTDIN- < 0.85 x Bias - FORCE- open\n"); 
            }
            if (fault & MAX31865_FAULT_OVUV) {
                printf("Under/Over voltage\n"); 
            }

            max31865_clear_fault(max31865_dev);
        }
    }
    ret = max31865_remove(max31865_dev);
    if (ret<0) return ret;

    return 0;
}


int main()
{  
    max31865_Init();
    return -999;
    // AD7706_Init();
    // return -1233;

    ad717x_dev *dev;
    struct ad9833_dev *ad9833_dev;

    int32_t cont_data = 0;
    int cont_counter = 0;
    int ret = 0;
    

    enum STATIC_DYNAMIC {
        STATIC_MODE,
        DYNAMIC_MODE,
        SINUSOID_WAVE_MODE,
        SQUARE_WAVE_MODE,

    };
    enum STATIC_DYNAMIC stat_dyn_choice = STATIC_MODE;

    switch (stat_dyn_choice) {
        case (STATIC_MODE):
            printf("Static Mode\n");
            ret = Static_Init(&dev);
            if (ret < 0) {
                printf("Error in Static Initialization.\n");
                return ret;
            }
            
            int32_t adc_data = 0;
            ret = Static_Read(dev, &adc_data);
            if(ret<0) {
                printf("Error in reading value.\n");
                return ret;
            }
            //double real_voltage = (adc_data/2) * (2.5/(0.75*8388608)) / 1.333333; // unipolar conversion
            double real_voltage = (double) ((((double) adc_data) - 8388608)*2.5) /(8388608) ;

            printf("real_voltage %f\n", real_voltage);
            

            break;
        
        case (DYNAMIC_MODE):
            // printf("Dynamic Mode\n");
            // int32_t cont_data = 0;
            // int cont_counter = 0;

            ret = Dynamic_Init(&dev);
            if (ret < 0) {
                printf("Error in reading value.\n");
                return ret;
            }

            double current_time = clock();
            // bcm2835_gpio_write(AD7175_2_CS_PIN, LOW);
            AD717X_ComputeDataregSize(dev);
            while (cont_counter < 10000) {
                ret = AD717X_WaitForReady(dev, AD717X_CONV_TIMEOUT);
                if (ret < 0)
                    return ret;
                ret = AD717X_ReadData(dev, &cont_data);
                if(ret<0) {
                    perror("Error reading value\n");
                    return ret;
                }
                // printf("ADC Raw Data (CONTINOUS)=%d\n", cont_data);
                // printf("ADC Raw Data (CONTINOUS)=%f\n", (cont_data/2) * (2.5/(0.75*8388608)) / 1.333333);
                cont_counter++;
            }

            // bcm2835_gpio_write(AD7175_2_CS_PIN, HIGH);
            
            double time_dif = clock() - current_time;

            printf("%f\n", time_dif/CLOCKS_PER_SEC);
            break;

        case (SINUSOID_WAVE_MODE):
            ret = Sinusoid_Init(&ad9833_dev);
            if (ret < -1) return ret;
            Sinusoid_Gen(ad9833_dev);
            Aux_Spi_Free(ad9833_dev->spi_dev);
            ad9833_remove(ad9833_dev);
            break;
        
        case (SQUARE_WAVE_MODE):
            ret = Square_Init(&ad9833_dev);
            if (ret < -1) return ret;

            Square_Gen(ad9833_dev);
            ad9833_remove(ad9833_dev);
            break;

        default:
            return -1;
    }


    // AD717X_remove(dev);
    // spi_free();

    return 0;
}

