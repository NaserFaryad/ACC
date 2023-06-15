#include "pigpio.h"
#include <QThread>
using namespace std;

int value = 0;
PiGPIO::PiGPIO(QObject *parent) : QThread(parent)
{

    read_flag = false;
    connect(&m_timer, &QTimer::timeout, this, &PiGPIO::timeout);

    m_timer.setInterval(1000);
    m_timer.start();
    m_display = "Timer Starting";
    qInfo() << m_display;
    emit notice(QVariant(m_display));
    int ret;

    sin_gen_flag = false;
    sqr_gen_flag = false;
    warning_toggle = false;


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
    try {
        int ret = 0;
        spi_device *spi_dev;
        struct ad9833_init_param init_param;

        ret = Aux_Spi_Init(&spi_dev, 50000, SPI_MODE_2, 1, SPIDEV_AUX_SPI_CS0);
        if (ret < 0)
            return AUX_SPI_INIT_ERROR;

        /* Set Relay ON */
        /* RELAY K1 */
        bcm2835_gpio_fsel(RELAY_K1_GPIO, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(RELAY_K1_GPIO, HIGH);  // in Square wave mode, the relay shoul be oOn
        bcm2835_delay(1);

        init_param.act_device = ID_AD9833;
        init_param.spi_dev = spi_dev;

        ret = ad9833_init(&ad9833_dev, init_param);
        if (ret < 0) {  // TODO: FIX ERROR CODE
            return AD9833_INIT_ERROR;
        }

        ad9833_out_mode(ad9833_dev, AD9833_OUT_SINUSOID);  // #TODO:
        return 0;

    } catch (const char* err) {
        qInfo() << "PiGPIO Thread: Sinusoid_Init, catched an error!, " << err;
        return SIN_INIT_ERROR;
    }

}

int PiGPIO::Square_Init()
{
    try {
        int ret = 0;
        spi_device *spi_dev;
        struct ad9833_init_param init_param;
        ret = Aux_Spi_Init(&spi_dev, 50000, SPI_MODE_2, 1, SPIDEV_AUX_SPI_CS0);
        if (ret < 0)
            return AUX_SPI_INIT_ERROR;
        /* Set Relay OFF */
        /* RELAY K1 */
        bcm2835_gpio_fsel(RELAY_K1_GPIO, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(RELAY_K1_GPIO, LOW);  // in Square wave mode, the relay shoul be off
        bcm2835_delay(1);
        init_param.act_device = ID_AD9833;
        init_param.spi_dev = spi_dev;
        ret = ad9833_init(&ad9833_dev, init_param);
        if (ret < 0) {  // TODO: FIX ERROR CODE
            return AD9833_INIT_ERROR;
        }
        // TODO: Duty cycle should be checked!
        ad9833_out_mode(ad9833_dev, AD9833_OUT_DAC_DATA_MSB);
        return 0;
    } catch (const char* err) {
        qInfo() << "PiGPIO Thread: Square_Init, catched an error!, " << err;
        return SQUARE_INIT_ERROR;
    }

}

int PiGPIO::AD7706_Init()
{
    int ret = Aux_Spi_Init(&ad770x_spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS1);
    if (ret < 0)
        return AUX_SPI_INIT_ERROR;
    ad7706Reset(ad770x_spi_dev);
    ad7706Init(ad770x_spi_dev, CHN_AIN2, CLK_DIV_1, UNIPOLAR, GAIN_1, UPDATE_RATE_50);
    ad7706Init(ad770x_spi_dev, CHN_AIN1, CLK_DIV_1, UNIPOLAR, GAIN_1, UPDATE_RATE_50);
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

void PiGPIO::setMutex(QMutex *newMutex)
{
    g_mutex = newMutex;
}

void PiGPIO::set_signal_status(bool *newStatus)
{
    signal_status = newStatus;
}

void PiGPIO::timeout()
{
    m_display = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
    const QMutexLocker locker(&m_mutex);
    read_flag = true;
    if(warning_toggle)
        toggle_warning(40);
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
//    const QMutexLocker locker(&m_mutex);
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
    double result1 = 0.0f;
    double result2 = 0.0f;
    int i = 0;
    sum_ch1 = 0;
    sum_ch2 = 0;
    for(i = 0; i< 12; i++){
        ret = readADResultDouble(ad770x_spi_dev, &result1,CHN_AIN1, 0, 1.225, UNIPOLAR, 1);
        if(ret < 0)
        {
            emit error_occured("PiGPIO Thread: read_current func, error code="+QString::number(AD7706_READ_TIMEOUT_ERROR));
            break;
        }
        sum_ch1 += result1;
    }
    for(i = 0; i< 12; i++){
        ret = readADResultDouble(ad770x_spi_dev, &result2,CHN_AIN2, 0, 1.225, UNIPOLAR, 1);
        if(ret < 0)
        {
            emit error_occured("PiGPIO Thread: read_current func, error code="+QString::number(AD7706_READ_TIMEOUT_ERROR));
            break;
        }
        sum_ch2 += result2;
    }
//    qInfo() << " ADC RAW VALUEs: " << sum_ch1 << ", " << sum_ch2;
    avg_ch1 = sum_ch1 / i;
    avg_ch2 = sum_ch2 / i;
    double opamp_gain = 5.6;
    current_P15 = ((avg_ch1 + 0) / 5.6) / opamp_gain;
    current_N15 = ((avg_ch2 + 0) / 5.6) / opamp_gain;
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
//           qInfo() << "PiGPIO Thread: read_temperature func, RTD High Threshold\n";
           emit temperature_fault_notice("PiGPIO Thread: read_temperature func,RTD High Threshold\n");
       }
   if (fault & MAX31865_FAULT_LOWTHRESH) {
//           qInfo() << "PiGPIO Thread: read_temperature func,RTD Low Threshold\n";
           emit temperature_fault_notice("PiGPIO Thread: read_temperature func,RTD Low Threshold\n");
       }
   if (fault & MAX31865_FAULT_REFINLOW) {
//           qInfo() << "PiGPIO Thread: read_temperature func, REFIN- > 0.85 x Bias\n";
           emit temperature_fault_notice("PiGPIO Thread: read_temperature func,REFIN- > 0.85 x Bias\n");
       }
   if (fault & MAX31865_FAULT_REFINHIGH) {
//           qInfo() << "PiGPIO Thread: read_temperature func, REFIN- < 0.85 x Bias - FORCE- open\n";
           emit temperature_fault_notice("PiGPIO Thread: read_temperature func, REFIN- < 0.85 x Bias - FORCE- open\n");
       }
   if (fault & MAX31865_FAULT_RTDINLOW) {
//           qInfo() << "PiGPIO Thread: read_temperature func, RTDIN- < 0.85 x Bias - FORCE- open\n";
           emit temperature_fault_notice("PiGPIO Thread: read_temperature func, RTDIN- < 0.85 x Bias - FORCE- open\n");
       }
   if (fault & MAX31865_FAULT_OVUV) {
//           qInfo() << "PiGPIO Thread: read_temperature func, Under/Over voltage\n";
           emit temperature_fault_notice("PiGPIO Thread: read_temperature func, Under/Over voltage\n");
       }

       max31865_clear_fault(max31865_dev);
       return -500;
   }
   return temp;
}

void PiGPIO::system_power_off()
{
    relays_off();
}


void PiGPIO::run()
{
    qInfo() << "<<<<<<<<<<  RUN Loop Started  >>>>>>>>>>";
    int ret;
    relay_init();
    pwr_5v_ok_on();
    ret = PiGPIO::Square_Init();
    if (ret < 0)
        emit error_occured("PiGPIO Thread: Square_Init func, error code="+QString::number(ret));
    wave_mode = SQUARE;
    ret = PiGPIO::AD7706_Init();
    if (ret < 0) {
        emit error_occured("PiGPIO Thread: AD7706_Init func, error code="+QString::number(ret));
    }
    ret = PiGPIO::max31865_Init();
    if (ret < 0) {
        emit error_occured("PiGPIO Thread: max31865_Init func, error code="+QString::number(MAX31865_INIT_ERROR));
    }

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
                ret = PiGPIO::Sinusoid_Init();
                if (ret < 0)
                    emit error_occured("PiGPIO Thread: Sinusoid_Init func, error code="+QString::number(ret));
                wave_mode = SIN;
            }
            else {
                ret = Aux_Spi_Init(&ad9833_dev->spi_dev, 50000, SPI_MODE_2, 1, SPIDEV_AUX_SPI_CS0);
                if (ret < 0)
                    emit error_occured("PiGPIO Thread: run loop, error code="+QString::number(AUX_SPI_INIT_ERROR));
                }
            qInfo() << "#########Sin Generator, Frequency= " << sin_freq;
            ad9833_set_freq(ad9833_dev, 0, sin_freq);
            ad9833_set_phase(ad9833_dev, 0, 0);
            ad9833_select_freq_reg(ad9833_dev, 0);
            ad9833_select_phase_reg(ad9833_dev, 0);
            sin_gen_flag = false;
            g_mutex->lock();
            *signal_status = true;
            g_mutex->unlock();
//            emit signal_generated();
            ret = Aux_Spi_Init(&max31865_spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS2);
            if (ret < 0)
                emit error_occured("PiGPIO Thread: run loop, error code="+QString::number(AUX_SPI_INIT_ERROR));
            ret = Aux_Spi_Init(&ad770x_spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS1);
            if (ret < 0)
                emit error_occured("PiGPIO Thread: run loop, error code="+QString::number(AUX_SPI_INIT_ERROR));
        }
        else if(sqr_gen_flag)
        {
            const QMutexLocker locker(&m_mutex);
            if (wave_mode != SQUARE)
            {
                ret = PiGPIO::Square_Init();
                if (ret < 0)
                    emit error_occured("PiGPIO Thread: Square_Init func, error code="+QString::number(ret));
                wave_mode = SQUARE;

            }
            else {
                ret = Aux_Spi_Init(&ad9833_dev->spi_dev, 50000, SPI_MODE_2, 1, SPIDEV_AUX_SPI_CS0);
                if (ret < 0)
                    emit error_occured("PiGPIO Thread: run loop, error code="+QString::number(AUX_SPI_INIT_ERROR));
            }
            qInfo() << "#########Sqr Generator, Frequency= " << sqr_freq;
            ad9833_set_freq(ad9833_dev, 0, sqr_freq);
            ad9833_set_phase(ad9833_dev, 0, 0.0);
            ad9833_select_freq_reg(ad9833_dev, 0);
            ad9833_select_phase_reg(ad9833_dev, 0);
            sqr_gen_flag = false;
            ret = Aux_Spi_Init(&max31865_spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS2);
            if (ret < 0)
                emit error_occured("PiGPIO Thread: run loop, error code="+QString::number(AUX_SPI_INIT_ERROR));
            ret = Aux_Spi_Init(&ad770x_spi_dev, 2500000, SPI_MODE_3, 0, SPIDEV_AUX_SPI_CS1);
            if (ret < 0)
                emit error_occured("PiGPIO Thread: run loop, error code="+QString::number(AUX_SPI_INIT_ERROR));
             }
        else if(read_flag)
        {
//            ret = PiGPIO::AD7706_Init();
//            if (ret < 0) {
//                emit error_occured("PiGPIO Thread: AD7706_Init func, error code="+QString::number(ret));
//            }
            float temp;
            QVariantList current_temp;
            QVariantList current;
            temp = PiGPIO::read_temperature();
            current = PiGPIO::read_current();
            current_temp.append(current[0]);
            current_temp.append(current[1]);
            if(current[1] > CURRENT_LIMIT || current[0] > CURRENT_LIMIT)
                warning_toggle = true;
            else
                warning_toggle = false;
            if(current[1] > CURRENT_THRESHOLD || current[0] > CURRENT_THRESHOLD)
                sensor_ok_on();
            else
                sensor_ok_off();
//            qInfo() << "Current: " << current[0]  << " , " << current[1] << " Temp: " << temp;
            current_temp.append(temp);
            emit print_current(current_temp);
//            const QMutexLocker locker(&m_mutex);
            read_flag = false;
        }

        }
}

