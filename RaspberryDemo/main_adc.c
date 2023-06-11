#include "main_adc.h"
#include "ad7175_2_regs.h"

int bcm2835_and_spi_init()
{
    if (!bcm2835_init())
       {
           return BCM2835_INIT_ERROR;
       }

       if (!bcm2835_spi_begin())
       {
           return BCM2835_SPI_BEGIN_ERROR;
       }

       bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
       bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);                   // The default
       bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128);
       bcm2835_gpio_fsel(AD7175_2_CS_PIN, BCM2835_GPIO_FSEL_OUTP);
       bcm2835_gpio_write(AD7175_2_CS_PIN, HIGH);
       return 0;
}

int spi_free()
{
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}

bool AD7175_VerifyChipID(ad717x_dev **dev)
{
    AD717X_ReadRegister(*dev, AD717X_ID_REG);
    ad717x_st_reg *pReg;
    pReg = AD717X_GetReg(*dev, AD717X_ID_REG);
    if (!pReg){
        fprintf(stderr,"error getting register\n");
        return false;
    }

    // AD7175-2 ID ===> 0X0CDX, 4 LSB IS DON'T CARE. SO WE SHIFT THE VALUE BY 4 TO IGNORE IT
    if(pReg->value >> 4 == (int32_t)AD7175_2_CHIP_ID) {
        return true;
    }
    return false;
}


void WriteRegister(ad717x_dev *dev, uint8_t addr, int32_t val)
{
    ad717x_st_reg *pReg;
    pReg = AD717X_GetReg(dev, addr);
    if (!pReg){
        fprintf(stderr,"error getting register\n");
        return;
    }
    pReg->value = val;
    AD717X_WriteRegister(dev, addr);
}

int Initialize_Device(ad717x_dev **device, ad717x_init_param init_param, int max_retry)
{
   int ret = 0;
   bool success_flag = false;
   uint8_t counter = 0;

   while(counter <= max_retry){
       ret = AD717X_Init(device, init_param);
       if (ret < 0) {
           fprintf(stderr,"Initialize_Device: error initialize %d \n", ret);
           ++counter;
       } else {
           success_flag = true;
           break;
       }
   }

   if (!success_flag)
       return AD717X_INIT_ERROR;

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
       return AD7175_ChipID_ERROR;

   return 0;
}
int zero_internal_calibration(int *offset)
{
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
    ad7175_2_init.mode = INTERNAL_OFFSET_CALIB;
    /* Initialize bcm2835 with SPI */
    ret = bcm2835_and_spi_init();
    if (ret < 0) {
        fprintf(stderr,"bcm2835_init or bcm2835_spi_begin failed.\n");
        return ret;
    }
    /* Set Relay off */

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
    ret = ad717x_set_channel_status(dev, CHANNEL_ID_0, true);  // enable channel 1
    if (ret == -2) return AD717X_COMM_ERROR;
    else if (ret == -3) return AD717X_TIMEOUT_ERROR;
    else if (ret < 0) return AD717X_SETCHNL_ERROR;

    ret = ad717x_set_channel_status(dev, CHANNEL_ID_1, false);  // disable channel 2
    if (ret == -2) return AD717X_COMM_ERROR;
    else if (ret == -3) return AD717X_TIMEOUT_ERROR;
    else if (ret < 0) return AD717X_SETCHNL_ERROR;

    ret = ad717x_set_channel_status(dev, CHANNEL_ID_2, false);  // disable channel 3
    if (ret == -2) return AD717X_COMM_ERROR;
    else if (ret == -3) return AD717X_TIMEOUT_ERROR;
    else if (ret < 0) return AD717X_SETCHNL_ERROR;

    ret = ad717x_set_channel_status(dev, CHANNEL_ID_3, false);  // disable channel 4
    if (ret == -2) return AD717X_COMM_ERROR;
    else if (ret == -3) return AD717X_TIMEOUT_ERROR;
    else if (ret < 0) return AD717X_SETCHNL_ERROR;

    /*Important: for AD7175-2 only "ainp" srtuct should be assigned, no need to "ad717x_analog_input_pairs" */
//    union ad717x_analog_inputs channel0Input;
//    channel0Input.ainp.pos_analog_input = AIN0;//AVDD_AVSS_P;
//    channel0Input.ainp.neg_analog_input = AIN4;//AVDD_AVSS_M;
//    ret = ad717x_connect_analog_input(dev, CHANNEL_ID_0, channel0Input);
//    if (ret < 0) return AD717x_CON_ANALOG_IN_ERROR;

    ret = AD717X_WaitForReady(dev, AD717X_CONV_TIMEOUT);
    if (ret < 0) return AD717X_TIMEOUT_ERROR;
    AD717X_ReadRegister(dev, AD717X_OFFSET0_REG);
    ad717x_st_reg *pReg;
    pReg = AD717X_GetReg(dev, AD717X_OFFSET0_REG);
    if (!pReg){
        fprintf(stderr,"error getting register\n");
        return false;
    }
    *offset = pReg->value;
    fprintf(stderr,"----------- Offset Value %d\n", pReg->value);
    return 0;
}
int Static_Init(ad717x_dev **device)
{

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
        if (ret < 0) {
            fprintf(stderr,"bcm2835_init or bcm2835_spi_begin failed.\n");
            return ret;
        }

        /* Set Relay off */

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
        if (ret == -2) return AD717X_COMM_ERROR;
        else if (ret == -3) return AD717X_TIMEOUT_ERROR;
        else if (ret < 0) return AD717X_SETCHNL_ERROR;

        ret = ad717x_set_channel_status(dev, CHANNEL_ID_1, false);  // disable channel 2
        if (ret == -2) return AD717X_COMM_ERROR;
        else if (ret == -3) return AD717X_TIMEOUT_ERROR;
        else if (ret < 0) return AD717X_SETCHNL_ERROR;

        ret = ad717x_set_channel_status(dev, CHANNEL_ID_2, false);  // disable channel 3
        if (ret == -2) return AD717X_COMM_ERROR;
        else if (ret == -3) return AD717X_TIMEOUT_ERROR;
        else if (ret < 0) return AD717X_SETCHNL_ERROR;

        ret = ad717x_set_channel_status(dev, CHANNEL_ID_3, false);  // disable channel 4
        if (ret == -2) return AD717X_COMM_ERROR;
        else if (ret == -3) return AD717X_TIMEOUT_ERROR;
        else if (ret < 0) return AD717X_SETCHNL_ERROR;

        /*Important: for AD7175-2 only "ainp" srtuct should be assigned, no need to "ad717x_analog_input_pairs" */
        union ad717x_analog_inputs channel0Input;
        channel0Input.ainp.pos_analog_input = AIN0;//AVDD_AVSS_P;
        channel0Input.ainp.neg_analog_input = AIN4;//AVDD_AVSS_M;
        ret = ad717x_connect_analog_input(dev, CHANNEL_ID_0, channel0Input);
        if (ret < 0) return AD717x_CON_ANALOG_IN_ERROR;

        ret = ad717x_set_polarity(dev, true, SETUP_ID_0);  // bipolar or unipolar -> true: bipolar, false: unipolar
        if (ret < 0) return AD717x_SET_POLARITY_ERROR;

        ret = ad717x_set_reference_source(dev, INTERNAL_REF, SETUP_ID_0);  // set INTERNAL_REF for setup 0
        if (ret < 0) return AD717x_SET_REF_SRC_ERROR;

        ret = ad717x_enable_input_buffer(dev, true, false, SETUP_ID_0);  // enable input and reference source
        if (ret < 0) return ret;

        ret = ad717x_configure_device_odr(dev, FILTER_ID_0, sps_503);  // Set sample rate
        if (ret != 0) return AD717x_CONF_DEV_ODR_ERROR;

        ret = ad717x_assign_setup(dev, CHANNEL_ID_0, SETUP_ID_0);  // Assign setup 0 to channel 0
        if (ret < 0) return AD717x_ASSIGN_SETUP_ERROR;

        WriteRegister(dev, AD717X_ADCMODE_REG, 0xA010);
        WriteRegister(dev, AD717X_FILTCON0_REG, 0x572);

        *device = dev;

        return ret;
}

int Static_Read(ad717x_dev *dev, int32_t *adc_data)
{

        int ret = 0;
        int32_t adc_data_temp = 0;
        int32_t adc_data_sum = 0;
        int counter = 0;
        while(counter < 5)
        {
            ret = ad717x_single_read(dev, CHANNEL_ID_0, &adc_data_temp);
            if (ret < 0) return AD717x_SINGLE_READ_ERROR;
            adc_data_sum += adc_data_temp;
            counter++;
        }
        *adc_data = adc_data_sum / counter;
        return 0;
}

int Dynamic_Init(ad717x_dev **device)
{
        fprintf(stderr, "ADC Initializing as Dynamic mode...\n");
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
           .oder = sinc5_sinc1,
           .odr = sps31250_e
       };
       ad7175_2_init.filter_configuration[0] = filtr;

       ret = bcm2835_and_spi_init();
       if (ret < 0) {
           printf("bcm2835_init or bcm2835_spi_begin failed.\n");
           return ret;
       }

       /* Set Relay off */

       /* RELAY K2 */
       bcm2835_gpio_fsel(RELAY_K2_GPIO, BCM2835_GPIO_FSEL_OUTP);
       bcm2835_gpio_write(RELAY_K2_GPIO, HIGH);
       bcm2835_delay(1);

       /* RELAY K3 */
       bcm2835_gpio_fsel(RELAY_K3_GPIO, BCM2835_GPIO_FSEL_OUTP);
       bcm2835_gpio_write(RELAY_K3_GPIO, LOW);
       bcm2835_delay(1);

       ret = Initialize_Device(&dev, ad7175_2_init, INITIALIZE_MAX_RETRY);
       if (ret < 0) return ret;


       /* Registers setup */

       WriteRegister(dev, AD717X_IFMODE_REG, 0x108);  // Interface Register

       ret = ad717x_set_channel_status(dev, CHANNEL_ID_0, true);  // enable channel 1
       if (ret == -2) return AD717X_COMM_ERROR;
       else if (ret == -3) return AD717X_TIMEOUT_ERROR;
       else if (ret < 0) return AD717X_SETCHNL_ERROR;
       ret = ad717x_set_channel_status(dev, CHANNEL_ID_1, false);  // disable channel 2
       if (ret == -2) return AD717X_COMM_ERROR;
       else if (ret == -3) return AD717X_TIMEOUT_ERROR;
       else if (ret < 0) return AD717X_SETCHNL_ERROR;
       ret = ad717x_set_channel_status(dev, CHANNEL_ID_2, false);  // disable channel 3
       if (ret == -2) return AD717X_COMM_ERROR;
       else if (ret == -3) return AD717X_TIMEOUT_ERROR;
       else if (ret < 0) return AD717X_SETCHNL_ERROR;
       ret = ad717x_set_channel_status(dev, CHANNEL_ID_3, false);  // disable channel 4
       if (ret == -2) return AD717X_COMM_ERROR;
       else if (ret == -3) return AD717X_TIMEOUT_ERROR;
       else if (ret < 0) return AD717X_SETCHNL_ERROR;

       /*Important: for AD7175-2 only "ainp" srtuct should be assigned, no need to "ad717x_analog_input_pairs" */
       union ad717x_analog_inputs channel0Input;
       channel0Input.ainp.pos_analog_input = AIN2;
       // channel0Input.ainp.pos_analog_input = AIN2; <-- this is final
       channel0Input.ainp.neg_analog_input = AIN4;
       ret = ad717x_connect_analog_input(dev, CHANNEL_ID_0, channel0Input);
       if (ret < 0) return AD717x_CON_ANALOG_IN_ERROR;


       ret = ad717x_set_polarity(dev, true, SETUP_ID_0);  // bipolar or unipolar -> true: bipolar, false: unipolar
       if (ret < 0) return AD717x_SET_POLARITY_ERROR;

       ret = ad717x_set_reference_source(dev, INTERNAL_REF, SETUP_ID_0);  // set INTERNAL_REF for setup 0
       if (ret < 0) return AD717x_SET_REF_SRC_ERROR;

       ret = ad717x_enable_input_buffer(dev, true, true, SETUP_ID_0);  // enable input and reference source
       if (ret < 0) return AD717x_ENABLE_IN_BUF_ERROR;

//       ret = ad717x_configure_device_odr(dev, FILTER_ID_0, sps31250_e);  // Set sample rate
//       if (ret < 0) return ret;


       ret = ad717x_assign_setup(dev, CHANNEL_ID_0, SETUP_ID_0);  // Assign setup 0 to channel 0
       if (ret < 0) return AD717x_ASSIGN_SETUP_ERROR;

       /* Set Mode to CONTINUOUS Conversion */
       ret = ad717x_set_adc_mode(dev, CONTINUOUS);
       if (ret < 0) return AD717x_SET_ADC_MODE_ERROR;

//       WriteRegister(dev, AD717X_FILTCON0_REG, 0x567);  // Filter 0 Register
       // PrintData(dev, AD717X_FILTCON0_REG);
       ret = AD717X_set_filter_type(dev, FILTER_ID_0, filtr);
       if (ret < 0) return AD717X_SET_FILTR_TYPE_ERROR;
       WriteRegister(dev, AD717X_ADCMODE_REG, 0xA000);  // ADC Mode Register

       // WriteRegister(dev, AD717X_IFMODE_REG, 0x148);  // Interface Register
       // PrintData(dev, AD717X_IFMODE_REG);

       *device = dev;

       return ret;
}

int Dynamic_Read(ad717x_dev *dev, int32_t *adc_data)
{
    int ret = 0;
    ret = AD717X_WaitForReady(dev, AD717X_CONV_TIMEOUT);
    if (ret ==  -3)
        return AD717X_TIMEOUT_ERROR;
    else if (ret < 0)
        return AD717X_WAITING_ERROR;
    ret = AD717X_ReadData(dev, adc_data);
    if (ret < 0) return AD717X_READ_DATA_ERROR;
    return ret;
}
