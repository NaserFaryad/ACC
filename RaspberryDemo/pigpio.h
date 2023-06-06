#ifndef PIGPIO_H
#define PIGPIO_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QVariant>

#include "spidev_lib.h"
#include "ad9833.h"
#include "ad770x.h"
#include "max31865.h"

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

class PiGPIO : public QObject
{
    Q_OBJECT
public:
    explicit PiGPIO(QObject *parent = nullptr);

    void gpio_export(QString pinNum, QString pinDirc);
    int Sinusoid_Init();
    int Square_Init();
    int AD7706_Init();
    int max31865_Init();

signals:
    void notice(QVariant data);
    void print_current(QVariantList current);
    void temperature_fault_notice(QString message);

private slots:

public slots:
    void start();
    void stop();
    void gpio_high(QString pinNum);
    void gpio_low(QString pinNum);
    void Sinusoid_Gen(int freq);
    void Square_Gen(int freq);
    QVariantList read_current();
    float read_temperature();
    void main_loop();

private:
    int wave_mode;
    bool loop_lock;
    bool read_flag;
    QTimer m_timer;
    QString m_display;
    struct ad9833_dev *ad9833_dev;
    spi_device *ad770x_spi_dev;
    spi_device *max31865_spi_dev;
    struct max31865_dev *max31865_dev;
    void timeout();

};

#endif // PIGPIO_H
