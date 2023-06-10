#include "bridge.h"


Bridge::Bridge(QObject *parent) : QObject(parent)
{
    button_name = -1;
    QVariantList config = files.read_Params();
    gain = config[0].toDouble();
    offset = config[1].toDouble();
    qInfo() << "calibration parameters: " << gain << ", " << offset;
    emit read_static_voltage(QVariant(72));

}

void Bridge::pressed_button(int value)
{
    button_name = value;
    qInfo() <<"pressed button slot inside bridge: " << value;
    emit read_static_voltage(QVariant(value));
}

void Bridge::button_values(double value)
{
    qInfo() <<"pressed button slot inside bridge: " << value;
    switch (button_name) {
        case GP:
            value = value * gain + offset;
            qInfo() <<"GP Calculating inside worker";
            emit gp_ready(value);
            break;
        case GN:
            value = value * gain + offset;
            qInfo() <<"GN Calculating inside worker";
            emit gn_ready(value);
            break;
        case ZP:
            value = value * gain + offset;
            emit zp_ready(value);
            break;
        case ZN:
            value = value * gain + offset;
            emit zn_ready(value);
            break;
        case P180:
            value = value * gain + offset;
            emit p180_ready(value);
            break;
        case N180:
            value = value * gain + offset;
            emit n180_ready(value);
            break;
        case P1CALIB:
            emit p1_captured(value);
            break;
        case P2CALIB:
            emit p2_captured(value);
            break;
        default:
            break;
    }
}

void Bridge::dynamic_test_values(QVariantList data)
{
    emit dynamic_test_ready(data);
}

void Bridge::static_real_time_value(double value)
{
    qInfo() << "Bridge>> Static Real Time Value: " << value;
    emit static_real_time_ready(value * gain + offset);
}

void Bridge::static_real_time_stop()
{
    qInfo() << "Bridge>> Static Real Time stoped " ;
    emit static_real_time_stoped();
}

void Bridge::static_real_time_start()
{
    qInfo() << "Bridge>> Static Real Time start. " ;
    emit static_real_time_started();
}

void Bridge::update_gain_offset()
{
    QVariantList config = files.read_Params();
    gain = config[0].toDouble();
    offset = config[1].toDouble();
}

void Bridge::dynamic_test(int freq, int wave)
{
    qInfo() << "Bridge: Dynamic Test Frequency: " << freq << " Wave Type: " << wave;
    emit start_dynamic_test(freq, wave);
}

void Bridge::time_date(QVariant data)
{
    emit date_time_ready(data);
}

void Bridge::xlsx_save(QString name_string)
{

//    files.xlsx_save(name_string);
    qInfo() << "Saved Successfully";
}

void Bridge::natural_freq_calc()
{
    emit start_natural_freq_calc();
}

void Bridge::cuttoff_freq_calc()
{
    emit start_cuttoff_freq_calc();
}

void Bridge::over_shoot_calc()
{
    emit start_over_shoot_calc();
}

void Bridge::sensor_current_temp(QVariantList current_temp)
{
    emit print_current_temp(current_temp);
}

void Bridge::overshoot(double over_shoot)
{
    emit overshoot_ready(over_shoot);
}

void Bridge::cutoff_slot(double cutof)
{
    emit cutoff_ready(cutof);
}

void Bridge::naturalfreq(double natural_freq)
{
    qInfo("Bridge: emit naturalfreq_ready");
    emit naturalfreq_ready(natural_freq);
}

void Bridge::progress_bar_value(int percent)
{
    emit set_progress_bar(percent);
}

void Bridge::print_error(QString msg)
{
    emit error_occured(msg);
}

void Bridge::dummy()
{
    emit dummy_signal();
}
