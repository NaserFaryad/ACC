#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QRandomGenerator>
#include <QVariant>
#include "main_adc.h"
#include <QElapsedTimer>
#include "bcm2835.h"
#include <QMutex>
#include <QMutexLocker>

#define ADC2DOUBLE_BIPOLAR(X) (double) ((((double) X) - 8388608)*2.5) /(8388608)

#define STATIC_MODE 0
#define DYNAMIC_MODE 1
#define GP 0
#define GN 1
#define ZP 2
#define ZN 3
#define P180 4
#define N180 5
#define P1CALIB 6
#define P2CALIB 7

// Signal Generator Wave Types
#define SQUARE 0
#define SIN    1

#define dynamic_sample_number   100
#define dynamic_sample_length   100
#define square_loop_number  2000
#define nf_freq_start   300
#define nf_freq_end     700
#define sf_freq_start   500
#define sf_freq_end     4500
#define NF_resolution 10
#define loop_limit 20
#define SF_resolution   50000
#define Overshoot_Freq_2  100
#define Overshoot_Freq_1  1
// =================== Acceptable ranges :
#define nf_min_acceptable   300
#define nf_max_acceptable   550

#define cf_min_acceptable   800
#define cf_max_acceptable   2500

#define Os_max_acceptable   35

/* GPIO Init*/
#define DYN_STA_SEL_PIN 22

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);

    int ADC_Init(int type);
    int32_t natural_freq_search(int start, int end, double start_value,double end_value, int loop_number);
    int32_t cutt_off_freq_search(int start, int end, double value_3db, int loop_number);
    int32_t capture_dynamic();
    int32_t capture_square();
    void setMutex(QMutex *newMutex);
    void set_signal_status(bool *newStatus);
    void set_signal_status_false();
    void wait_ready_generator();


signals:
    void finished();
    void generated(double data);
    void error_occured(QString msg);
    void adc_ready(double data);
    void static_real_time_ready(double data);
    void start_sinusoid_gen(int freq);
    void start_square_gen(int freq);
    void dynamic_test_ready(QVariantList data);
    void overshoot_ready(double over_shoot);
    void cutoff_ready(double cuttoff);
    void naturalfreq_ready(double natural_frq);
    void set_progress_bar(int percent);
    void timer_start();
    void timer_stop();

public slots:
    void static_real_time_start();
    void static_real_time_stop();
    void quit();
    void read_static_voltage(QVariant button_name);
    void dynamic_test(int freq, int wave);
    void natural_freq_calc();
    void cuttoff_freq_calc();
    void over_shoot_calc();
    void internal_calibration();
    bool signal_is_ready();





private:
    QTimer *m_timer;
    QElapsedTimer e_timer;
    QMutex *m_mutex;
    int adc_offset;
    bool m_producer;
    bool *signal_status; // true: ready, false: not generated
    int m_count;
    int adc_mode = -1;
    ad717x_dev *ad7175_device;
    void static_real_time();

};

#endif // WORKER_H
