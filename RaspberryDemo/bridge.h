#ifndef BRIDGE_H
#define BRIDGE_H

#include <QObject>
#include <QVariant>
#include <QDebug>
#include "file_manager.h"

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




class Bridge : public QObject
{
    Q_OBJECT
public:
    explicit Bridge(QObject *parent = nullptr);

    double static_voltage(double);

signals:
    void gp_ready(QVariant sVolt);
    void gn_ready(QVariant sVolt);
    void zp_ready(QVariant sVolt);
    void zn_ready(QVariant sVolt);
    void p180_ready(QVariant sVolt);
    void n180_ready(QVariant sVolt);
    void p1_captured(QVariant sVolt);
    void p2_captured(QVariant sVolt);
    void static_real_time_ready(QVariant sVolt);
    void read_static_voltage(QVariant button_name);
    void static_real_time_started();
    void static_real_time_stoped();
    void temperature(QVariant temp);
    void error_occured(QString msg);
    void start_dynamic_test(int freq, int wave);
    void date_time_ready(QVariant DateTime);
    void dynamic_test_ready(QVariantList data);
    void start_natural_freq_calc();
    void print_current_temp(QVariantList current_temp);
    void start_cuttoff_freq_calc();
    void start_over_shoot_calc();
    void overshoot_ready(double over_shoot);
    void cutoff_ready(double cutof);
    void naturalfreq_ready(double natural_freq);
    void set_progress_bar(int percent);
    void start_internal_calibration();
    void temperature_fault_notice(QString msg);
    void relay_power_off();

public slots:
    void pressed_button(int value);
    void button_values(double value);
    void dynamic_test_values(QVariantList data);
    void static_real_time_value(double value);
    void static_real_time_stop();
    void static_real_time_start();
    void update_gain_offset();
    void dynamic_test(int freq, int wave);
    void time_date(QVariant data);
    void xlsx_save(QString name_string);
    void natural_freq_calc();
    void cuttoff_freq_calc();
    void over_shoot_calc();
    void sensor_current_temp(QVariantList current_temp);
    void overshoot(double over_shoot);
    void cutoff_slot(double cutof);
    void naturalfreq(double natural_freq);
    void progress_bar_value(int percent);
    void print_error(QString msg);
    void internal_calibration();
    void temperature_fault(QString msg);
    void system_power_off();

private:
    int button_name;
    file_manager files;
    double gain;
    double offset;


};

#endif // BRIDGE_H
