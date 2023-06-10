#include "pigpio.h"
#include <QThread>

int value = 0;
PiGPIO::PiGPIO(QObject *parent) : QThread(parent)
{

    PiGPIO::gpio_export( "3", "out");
    loop_lock = false;
    read_flag = false;
    connect(&m_timer, &QTimer::timeout, this, &PiGPIO::timeout);
//    PiGPIO::start();
//    connect(&m_timer, &QTimer::timeout, this, &PiGPIO::main_loop);
    m_timer.setInterval(1000);
     m_timer.start();
    m_display = "Starting";
    qInfo() << m_display;
    emit notice(QVariant(m_display));
    int ret;
    ret = PiGPIO::Square_Init();
    if (ret < 0) {
        qInfo() << "Signal Generator Square wave Inint failed!!.";
    }
    wave_mode = SQUARE;
    qInfo() << "###### after sig gen init.";
    ret = PiGPIO::AD7706_Init();
    if (ret < 0) {
        qInfo() << "Current sensor Inint failed!!.";
    }
    PiGPIO::max31865_Init();
    sin_gen_flag = false;
    sqr_gen_flag = false;


//    PiGPIO::exec();

}


void PiGPIO::gpio_export(QString pinNum, QString pinDirc)
{
    FILE *sysfs_handle = nullptr;
    if((sysfs_handle = fopen("/sys/class/gpio/export","w")) != nullptr)
    {
        fwrite(pinNum.toUtf8().constData(),sizeof (char),2,sysfs_handle);
        fclose(sysfs_handle);
    }
    if((sysfs_handle = fopen("/sys/class/gpio/gpio"+pinNum.toUtf8()+"/direction","w")) != nullptr)
    {
        fwrite(pinDirc.toUtf8().constData(),sizeof (char),4,sysfs_handle);
        fclose(sysfs_handle);
    }
}

int PiGPIO::Sinusoid_Init()
{
    qInfo() << "PiGPIO: Sinusoid_Init function";
    int ret = 0;
    spi_device *spi_dev;
    struct ad9833_init_param init_param;

    ret = Aux_Spi_Init(&spi_dev, 50000, SPI_MODE_2, 1, SPIDEV_AUX_SPI_CS0);
    if (ret < 0) {
        return -1;
    }

    /* Set Relay ON */
    /* RELAY K1 */
    bcm2835_gpio_fsel(RELAY_K1_GPIO, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY_K1_GPIO, HIGH);  // in Square wave mode, the relay shoul be oOn
    bcm2835_delay(1);

    init_param.act_device = ID_AD9833;
    init_param.spi_dev = spi_dev;

    ret = ad9833_init(&ad9833_dev, init_param);
    if (ret < -1) {  // TODO: FIX ERROR CODE
        return -2;
    }

    ad9833_out_mode(ad9833_dev, AD9833_OUT_SINUSOID);  // #TODO:

    return 0;
}

int PiGPIO::Square_Init()
{
    qInfo() << "PiGPIO: Square wave Init.";
    int ret = 0;
    spi_device *spi_dev;
    struct ad9833_init_param init_param;
    qInfo() << "Aux SPI Init";
    ret = Aux_Spi_Init(&spi_dev, 50000, SPI_MODE_2, 1, SPIDEV_AUX_SPI_CS0);
    if (ret < 0) {
        return -1;
    }
    qInfo() << "PiGPIO: Aux SPI successfullt inited.";
    /* Set Relay OFF */
    /* RELAY K1 */
    bcm2835_gpio_fsel(RELAY_K1_GPIO, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY_K1_GPIO, LOW);  // in Square wave mode, the relay shoul be off
    bcm2835_delay(1);
    qInfo() << "PiGPIO: GPIO successfullt inited.";
    init_param.act_device = ID_AD9833;
    init_param.spi_dev = spi_dev;
    qInfo() << "PiGPIO: ad9833_init...";
    ret = ad9833_init(&ad9833_dev, init_param);
    qInfo() << "PiGPIO: ad9833_init successfullt inited.";
    if (ret < -1) {  // TODO: FIX ERROR CODE
        return -2;
    }
    // TODO: Duty cycle should be checked!
    ad9833_out_mode(ad9833_dev, AD9833_OUT_DAC_DATA_MSB);

    return 0;
}

int PiGPIO::AD7706_Init()
{
    int ret = Aux_Spi_Init(&ad770x_spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS_NONE);
    if (ret < 0)
        return ret;
    ad7706Reset(ad770x_spi_dev);
    ad7706Init(ad770x_spi_dev, CHN_AIN1, CLK_DIV_1, UNIPOLAR, GAIN_1, UPDATE_RATE_50);
    ad7706Init(ad770x_spi_dev, CHN_AIN2, CLK_DIV_1, UNIPOLAR, GAIN_1, UPDATE_RATE_50);
    return 0;
}

int PiGPIO::max31865_Init()
{
    int ret = 0;

    struct max31865_init_param init_param;

    ret = Aux_Spi_Init(&max31865_spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS2);
    if (ret<0) return ret;

    init_param.spi_init = max31865_spi_dev;
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
    return 0;
}

void PiGPIO::timeout()
{
//    float temp;
//    QVariantList current_temp;
//    QVariantList current;
    m_display = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
    const QMutexLocker locker(&m_mutex);
    read_flag = true;
//    qInfo() << "READ FLAG " << read_flag;
//    current = PiGPIO::read_current();
//    temp = PiGPIO::read_temperature();
    qInfo() << this << "******PiGPIO: " << QThread::currentThreadId();
//    current_temp.append(current[0]);
//    current_temp.append(current[1]);
////    qInfo() << "Current: " << current[1];
////    qInfo() << "temp: " << temp;
//    current_temp.append(temp);
//    emit print_current(current_temp);
//    read_flag = false;
    emit notice(QVariant(m_display));


}

void PiGPIO::timer_start()
{
    m_timer.start();
}

void PiGPIO::stop()
{
    m_timer.stop();

}

void PiGPIO::gpio_high(QString pinNum)
{
    FILE *sysfs_handle = nullptr;
    if((sysfs_handle = fopen("/sys/class/gpio/gpio"+pinNum.toUtf8()+"/value","w")) != nullptr)
    {
        fwrite("1", sizeof(char),2,sysfs_handle);
        fclose(sysfs_handle);
    }
}

void PiGPIO::gpio_low(QString pinNum)
{
    FILE *sysfs_handle = nullptr;
    if((sysfs_handle = fopen("/sys/class/gpio/gpio"+pinNum.toUtf8()+"/value","w")) != nullptr)
    {
        fwrite("0", sizeof(char),2,sysfs_handle);
        fclose(sysfs_handle);
    }
}

void PiGPIO::Sinusoid_Gen(int freq)
{
    const QMutexLocker locker(&m_mutex);
    sin_gen_flag = true;
    sin_freq = freq;
}

void PiGPIO::Square_Gen(int freq)
{
    const QMutexLocker locker(&m_mutex);
    sqr_gen_flag = true;
    sqr_freq = freq;
}

QVariantList PiGPIO::read_current()
{
    int ret = 0;
    double sum_ch1 = 0;
    double sum_ch2 = 0;
    double avg_ch1 = 0;
    double avg_ch2 = 0;
    double current_P15 = 0;
    double current_N15 = 0;

    int i = 0;
    sum_ch1 = 0;
    sum_ch2 = 0;
    for(i = 0; i< 12; i++){
        sum_ch1 = sum_ch1 + readADResultDouble(ad770x_spi_dev, CHN_AIN1, 0, 1.225);
    }
    for(i = 0; i< 12; i++){
        sum_ch2 = sum_ch2 + readADResultDouble(ad770x_spi_dev, CHN_AIN2, 0, 1.225);
    }
    double stop_time = clock();
    avg_ch1 = sum_ch1 / i;
    avg_ch2 = sum_ch2 / i;
    current_P15 = (avg_ch1 + 0) / 5.6;
    current_N15 = (avg_ch2 + 0) / 5.6;
    QVariantList current;
    current.append(current_P15);
    current.append(current_N15);
    return current;
}

float PiGPIO::read_temperature()
{
    uint16_t raw_val = 0;
    uint8_t fault;
    int ret = max31865_read_rtd(max31865_dev, &raw_val);
//    if(ret<0) return ret;
    float temp = calculateTemperature(raw_val, RNOMINAL, RREF);

    /* Fault Detection*/
    fault = max31865_read_fault(max31865_dev, MAX31865_FAULT_AUTO);
   if (fault) {
       emit temperature_fault_notice("Fault " + fault);
   if (fault & MAX31865_FAULT_HIGHTHRESH) {
           qInfo() << "RTD High Threshold\n";
           emit temperature_fault_notice("RTD High Threshold\n");
       }
   if (fault & MAX31865_FAULT_LOWTHRESH) {
           qInfo() << "RTD Low Threshold\n";
           emit temperature_fault_notice("RTD Low Threshold\n");
       }
   if (fault & MAX31865_FAULT_REFINLOW) {
           qInfo() << "REFIN- > 0.85 x Bias\n";
           emit temperature_fault_notice("REFIN- > 0.85 x Bias\n");
       }
   if (fault & MAX31865_FAULT_REFINHIGH) {
           qInfo() << "REFIN- < 0.85 x Bias - FORCE- open\n";
           emit temperature_fault_notice("REFIN- < 0.85 x Bias - FORCE- open\n");
       }
   if (fault & MAX31865_FAULT_RTDINLOW) {
           qInfo() << "RTDIN- < 0.85 x Bias - FORCE- open\n";
           emit temperature_fault_notice("RTDIN- < 0.85 x Bias - FORCE- open\n");
       }
   if (fault & MAX31865_FAULT_OVUV) {
           qInfo() << "Under/Over voltage\n";
           emit temperature_fault_notice("Under/Over voltage\n");
       }

       max31865_clear_fault(max31865_dev);
   }
   return temp;
}

void PiGPIO::run()
{
    qInfo() << "Inside EXEC Loop ////////////////////////////////";



//    forever {
//            if ( QThread::currentThread()->isInterruptionRequested() ) {
//                qInfo() << ">>>>>>>>>>>>>>Interrrupt Loop.";
//                return;
//            }
//    }
    while(true)
    {
        if ( QThread::currentThread()->isInterruptionRequested() ) {
                                return;
                    }

        if(sin_gen_flag)
        {

            const QMutexLocker locker(&m_mutex);
            if (wave_mode != SIN)
            {
                PiGPIO::Sinusoid_Init();
                wave_mode = SIN;
                qInfo() << "Initializing Sinusoid Wave...";
            }
            else {
                int ret = Aux_Spi_Init(&ad9833_dev->spi_dev, 50000, SPI_MODE_2, 1, SPIDEV_AUX_SPI_CS0);
//                if (ret < 0) {
//                    return -1;
                }
            qInfo() << "#########Sin Generator, Frequency= " << sin_freq;
            ad9833_set_freq(ad9833_dev, 0, sin_freq);
            ad9833_set_phase(ad9833_dev, 0, 0);
            ad9833_select_freq_reg(ad9833_dev, 0);
            ad9833_select_phase_reg(ad9833_dev, 0);
            sin_gen_flag = false;
            int ret = Aux_Spi_Init(&max31865_spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS2);
            ret = Aux_Spi_Init(&ad770x_spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS_NONE);
//            if (ret<0) return ret;
        }
        else if(sqr_gen_flag)
        {
            const QMutexLocker locker(&m_mutex);
            if (wave_mode != SQUARE)
            {
                qInfo() << "Initializing Square Wave...";
                PiGPIO::Square_Init();
                wave_mode = SQUARE;

            }
            else {
                int ret = Aux_Spi_Init(&ad9833_dev->spi_dev, 50000, SPI_MODE_2, 1, SPIDEV_AUX_SPI_CS0);
//                if (ret < 0) {
//                    return -1;
//                }
            }
            qInfo() << "#########Sqr Generator, Frequency= " << sqr_freq;
            ad9833_set_freq(ad9833_dev, 0, sqr_freq);
        //    qInfo() << "Square Wave setting phase";
            ad9833_set_phase(ad9833_dev, 0, 0.0);
            ad9833_select_freq_reg(ad9833_dev, 0);
            ad9833_select_phase_reg(ad9833_dev, 0);
            sqr_gen_flag = false;
            int ret = Aux_Spi_Init(&max31865_spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS2);
            ret = Aux_Spi_Init(&ad770x_spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS_NONE);
//            if (ret<0) return ret;

        }
        else if(read_flag)
        {
            float temp;
            QVariantList current_temp;
            QVariantList current;
            qInfo() << "Main Loop.";
            temp = PiGPIO::read_temperature();
            qInfo() << "temp: " << temp;
            current = PiGPIO::read_current();
            current_temp.append(current[0]);
            current_temp.append(current[1]);
            qInfo() << "Current: " << current[1];

            current_temp.append(temp);
            emit print_current(current_temp);
//            const QMutexLocker locker(&m_mutex);
            read_flag = false;
        }

        }
}

void PiGPIO::dummy()
{
    qInfo() << "OOOOOOOOOOOOOOOOOOOOO Inside dummy";
}
