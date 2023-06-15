#ifndef PIGPIO_H
#define PIGPIO_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QVariant>
#include <QThread>
#include <QMutex>

#include "spidev_lib.h"
#include "ad9833.h"
#include "ad770x.h"
#include "max31865.h"
#include "file_gpio.h"

#define RELAY_K1_GPIO 6
#define RELAY_K2_GPIO 22
#define RELAY_K3_GPIO 14

/* AD9833 */
#define AD9833_OUT_SINUSOID 0
#define AD9833_OUT_TRIANGLE 1
#define AD9833_OUT_DAC_DATA_MSBDIV2 2
#define AD9833_OUT_DAC_DATA_MSB 3

#define SQUARE 0
#define SIN    1

/*Error Codes*/
#define AUX_SPI_INIT_ERROR          -1
#define MAX31865_INIT_ERROR         -2
#define AD7706_READ_TIMEOUT_ERROR   -3
#define AD9833_INIT_ERROR           -4
#define SQUARE_INIT_ERROR           -5
#define SIN_INIT_ERROR              -6

/*current upper limit*/
#define CURRENT_LIMIT       0.030 // 30 mA
#define CURRENT_THRESHOLD   0.010 // 10 mA


class PiGPIO : public QThread
{
    Q_OBJECT
public:
    explicit PiGPIO(QObject *parent = nullptr);

    void gpio_export(QString pinNum, QString pinDirc);
    int Sinusoid_Init();
    int Square_Init();
    int AD7706_Init();
    int max31865_Init();
    void setMutex(QMutex *newMutex);
    void set_signal_status(bool *newStatus);

signals:
    void notice(QVariant data);
    void print_current(QVariantList current);
    void temperature_fault_notice(QString message);
    void error_occured(QString msg);
    void signal_generated();


private slots:

public slots:
    void timer_start();
    void stop();
    void gpio_high(QString pinNum);
    void gpio_low(QString pinNum);
    void Sinusoid_Gen(int freq);
    void Square_Gen(int freq);
    QVariantList read_current();
    float read_temperature();
    void system_power_off();



private:
    void run();
    int wave_mode;
    QMutex m_mutex;
    QMutex *g_mutex;
    bool read_flag;
    bool warning_toggle;
    bool sin_gen_flag;
    bool sqr_gen_flag;
    bool *signal_status;
    int sin_freq;
    int sqr_freq;
    QTimer m_timer;
    QString m_display;
    struct ad9833_dev *ad9833_dev;
    spi_device *ad770x_spi_dev;
    spi_device *max31865_spi_dev;
    struct max31865_dev *max31865_dev;
    void timeout();

};

#endif // PIGPIO_H
