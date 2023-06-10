#include "worker.h"
#include "ad717x.h"
#include <QThread>

Worker::Worker(QObject *parent) : QObject(parent)
{
    m_producer = false;
    m_count = 0;

}


void Worker::static_real_time_stop()
{
    m_producer = false;
}


int Worker::ADC_Init(int type)
{
    int ret_int = 0;
    if (adc_mode == type)
        return 0;
    if (type == STATIC_MODE)
    {
        ret_int = Static_Init(&ad7175_device);
        if (ret_int < 0)
            emit error_occured("Worker Thread: ADC_Init func, error code="+QString::number(ret_int));
        else {
            adc_mode = STATIC_MODE;
        }
        qInfo() << this << "###########Static mode initialized";
    }
    else if (type == DYNAMIC_MODE)
    {
        ret_int = Dynamic_Init(&ad7175_device);
        if (ret_int < 0)
            emit error_occured("Worker Thread: Dynamic_Init func, error code="+QString::number(ret_int));
        else {
            adc_mode = DYNAMIC_MODE;
        }
        qInfo() << this << "###########Dynamic mode initialized";
    }
    return ret_int;
}


int32_t Worker::natural_freq_search(int start, int end, double start_value, double end_value, int loop_number)
{
    if((end - start) < NF_resolution)
        return end;

    int     range;
    double  data[5];
    int     index[5];
    range = end - start;
    loop_number = loop_number + 1;

    index[0] = start;
    index[1] = start + (1 * range / 4);
    index[2] = start + (2 * range / 4);
    index[3] = start + (3 * range / 4);
    index[4] = end;

    data[0] = start_value;

    emit start_sinusoid_gen(index[1]);
    bcm2835_delayMicroseconds(20);
    data[1] = Worker::capture_dynamic();

    emit start_sinusoid_gen(index[2]);
    bcm2835_delayMicroseconds(20);
    data[2] = Worker::capture_dynamic();

    emit start_sinusoid_gen(index[3]);
    bcm2835_delayMicroseconds(20);
    data[3] = Worker::capture_dynamic();
    data[4] = end_value;

    qInfo() << "Natural Frequency, Searching Between " << start << " Hz and " << end << " Hz ... ";

    // sortng the data to find maximum interval
    if(loop_number >= loop_limit)
        return 0;
    // ================= data[0] is Max
    else if ((data[0]> data[1]) && (data[0]> data[2]) && (data[0]> data[3]) && (data[0]> data[4]))
            return Worker::natural_freq_search(index[0],index[1],data[0],data[1],loop_number);
    // ================= data[1] is Max
    else if((data[1]>data[0]) && (data[1] > data[2]) && (data[1] > data[3]) && (data[1] > data[4])){
        if(data[0]>data[2])
            return Worker::natural_freq_search(index[0],index[1],data[0],data[1],loop_number);
        else
            return Worker::natural_freq_search(index[1],index[2],data[1],data[2],loop_number);
    }
    // ================= data[2] is Max
    else if((data[2]>data[0]) && (data[2] > data[1]) && (data[2] > data[3]) && (data[2] > data[4])){
        if(data[1]>data[3])
            return Worker::natural_freq_search(index[1],index[2],data[1],data[2],loop_number);
        else
            return Worker::natural_freq_search(index[2],index[3],data[2],data[3],loop_number);
    }
    // ================= data[3] is Max
    else if((data[3]>data[0]) && (data[3] > data[1]) && (data[3] > data[2]) && (data[3] > data[4])){
        if(data[2]>data[4])
            return Worker::natural_freq_search(index[2],index[3],data[2],data[3],loop_number);
        else
            return Worker::natural_freq_search(index[3],index[4],data[3],data[4],loop_number);
    }
    // ================= data[4] is Max
    else
            return Worker::natural_freq_search(index[3],index[4],data[3],data[4],loop_number);
    return -1;
}

int32_t Worker::cutt_off_freq_search(int start, int end, double value_3db, int loop_number)
{
    int mid;
    double value_mid = 0;
    mid = (start+end)/2;

    qInfo() << "Cutt-Off Frequency; Searching Between " << start << " Hz and " << end << " Hz ... ";
    emit start_sinusoid_gen(mid);
    bcm2835_delayMicroseconds(20);
    value_mid = Worker::capture_dynamic();
    loop_number = loop_number + 1;
    if (loop_number >= loop_limit)
       return 0;
    else if(value_mid > value_3db){
        if(value_mid < (value_3db + SF_resolution))
            return   mid;
        else
            return Worker::cutt_off_freq_search(mid,end,value_3db,loop_number);
    }
    else{
        if(value_mid >  (value_3db - SF_resolution) )
                return mid;
        else
            return Worker::cutt_off_freq_search(start,mid,value_3db,loop_number);
    }
    return -1;
}

int32_t Worker::capture_dynamic()
{
    int ret = Worker::ADC_Init(DYNAMIC_MODE);
    if (ret < 0) {
        qInfo() <<"capture_dynamic: bcm2835_init or bcm2835_spi_begin failed.\n";
        emit error_occured("Worker Thread: capture_dynamic, ADC_Init, error code="+QString::number(ret));
        return ret;
    }
    else {
        adc_mode = DYNAMIC_MODE;
    }
    double sample_number = dynamic_sample_number;
    double sample_length = dynamic_sample_length;
    double final_min, final_max;
    int min_data =  8388607;
    int max_data = -8388607;

    int64_t min_sum =  0;
    int64_t max_sum =  0;


    int32_t Value = 0;

    int32_t range = 0;

    AD717X_ComputeDataregSize(ad7175_device);
    for(int i = 0; i < sample_number; i++){
        emit set_progress_bar(i/20);
        min_data =  8388607;
        max_data = -8388607;
        for(int j = 0 ; j < sample_length ; j++){
            Value = 0;
            int ret = Dynamic_Read(ad7175_device, &Value);
            if (ret < 0)
            {
                emit error_occured("Worker Thread: capture_dynamic, Dynamic_Read, error code="+QString::number(ret));
                qInfo() << "capture_dynamic: read ADC value failed. error code: " << ret;
                return ret;
            }
            if (Value & 0x800000)
                Value |= 0xFF000000;
            if(Value < min_data)
               min_data = Value;
            if(Value > max_data)
                max_data = Value;
        }
        min_sum = min_sum + min_data;
        max_sum = max_sum + max_data;
    }
    emit set_progress_bar(100);
    if(sample_number > 0){
        final_min = min_sum / sample_number;
        final_max = max_sum / sample_number;
        range = int32_t(final_max - final_min);
    }
    return range;

}

int32_t Worker::capture_square()
{
    int ret = Worker::ADC_Init(DYNAMIC_MODE);
    if (ret < 0) {
        emit error_occured("Worker Thread: capture_square, ADC_Init, error code="+QString::number(ret));
        qInfo() <<"capture_dynamic: bcm2835_init or bcm2835_spi_begin failed.\n";
        return ret;
    }
    else {
        adc_mode = DYNAMIC_MODE;
    }
    double sample_number = square_loop_number;

    int64_t max_sum =  0;
    int max_counter = 0;
    int min_counter = 0;
    int64_t min_sum =  0;

    double avg_min = 0;
    double avg_max = 0;

    int32_t Value = 0;

    int32_t range = 0;

    AD717X_ComputeDataregSize(ad7175_device);
    for(int i = 0; i < sample_number; i++){
//        ui->prog_bar->setValue(i/20);
        emit set_progress_bar(i/20);
        Value = 0;
        int ret = Dynamic_Read(ad7175_device, &Value);
        if (ret < 0)
        {
            emit error_occured("Worker Thread: capture_square, Dynamic_Read, error code="+QString::number(ret));
            qInfo() << "capture_square: read ADC value failed. error code: " << ret;
            i = sample_number;
        }
        if (Value & 0x800000)
            Value |= 0xFF000000;

        if(Value > 0){
            max_sum = max_sum + Value;
            max_counter = max_counter + 1;
        }
        else {
           min_sum = min_sum + Value;
           min_counter = min_counter + 1;
        }
    }
    emit set_progress_bar(100);
    if(min_counter > 0 )
        avg_min = min_sum / min_counter;
    if(max_counter > 0)
        avg_max = max_sum / max_counter;
    range = int32_t(avg_max - avg_min);

    return range;

}


void Worker::static_real_time_start()
{
    qInfo() << this << "Starting" << QThread::currentThread();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout,this,&Worker::static_real_time);
    int ret = Worker::ADC_Init(STATIC_MODE);
    if (ret < 0) {
        emit error_occured("Worker Thread: static_real_time_start, ADC_Init, error code="+QString::number(ret));
        qInfo() <<"bcm2835_init or bcm2835_spi_begin failed.\n";
    }
    m_producer = true;
    m_timer->setInterval(1000);
    m_timer->start();

}

void Worker::quit()
{
    qInfo() << this << "Quitting" << QThread::currentThread();
    m_timer->stop();
}


void Worker::read_static_voltage(QVariant button_name)
{
    qInfo() <<"read_static_voltage called inside worker";
    int ret;
    if (adc_mode != STATIC_MODE)
    {
        ret = Worker::ADC_Init(STATIC_MODE);
        if (ret < 0) {
            emit error_occured("Worker Thread: read_static_voltage, ADC_Init, error code="+QString::number(ret));
            qInfo() <<"bcm2835_init or bcm2835_spi_begin failed.\n";
        }
        else {
            adc_mode = STATIC_MODE;
        }

    }
    e_timer.start();
    int32_t  adc_data = 0;
    ret = Static_Read(ad7175_device, &adc_data);
    qInfo() << " Elapsed Time: " << e_timer.elapsed() ;
    qInfo() << "ADC RAW DATA= " <<  adc_data ;
    if(ret<0) {
        qInfo() << "Error reading value. Error Code: " << ret;
        emit error_occured("Worker Thread: read_static_voltage, Static_Read, error code="+QString::number(ret));
    }
//    double real_voltage = (adc_data/2) * (2.5/(0.75*8388608)) / 1.333333; // unipolar conversion
    double real_voltage = (double) ((((double) adc_data) - 8388608)*2.5) /(8388608) ;
    qInfo() << "Real Voltage after conversion: " << real_voltage;
    switch (button_name.toInt()) {
    case GP:
        qInfo() <<"GP Calculating inside worker";
        emit adc_ready(real_voltage);
        break;
    case GN:
        qInfo() <<"GN Calculating inside worker";
        emit adc_ready(real_voltage);
        break;
    case ZP:
        qInfo() <<"ZP Calculating inside worker";
        emit adc_ready(real_voltage);
        break;
    case ZN:
        qInfo() <<"ZN Calculating inside worker";
        emit adc_ready(real_voltage);
        break;
    case P180:
        qInfo() <<"P180 Calculating inside worker";
        emit adc_ready(real_voltage);
        break;
    case N180:
        qInfo() <<"N180 Calculating inside worker";
        emit adc_ready(real_voltage);
        break;
    case P1CALIB:
        qInfo() <<"P1CALIB Calculating inside worker";
        emit adc_ready(real_voltage);
        break;
    case P2CALIB:
        qInfo() <<"P2CALIB Calculating inside worker";
        emit adc_ready(real_voltage);
        break;
    default:
        break;
    }

}

void Worker::dynamic_test(int freq, int wave)
{
//    emit timer_stop();
    qInfo() << "Worker: dynamic test freq: " << freq << " wave: " << wave;
    if (wave == SQUARE)
        emit start_square_gen(freq);
    else if(wave == SIN)
        emit start_sinusoid_gen(freq);
    int ret = Worker::ADC_Init(DYNAMIC_MODE);
    if (ret < 0) {
        emit error_occured("Worker Thread: dynamic_test, ADC_Init, error code="+QString::number(ret));
        qInfo() <<"Dynamic init: bcm2835_init or bcm2835_spi_begin failed.\n";
    }
    else {
        adc_mode = DYNAMIC_MODE;
    }


    double sample_number = dynamic_sample_number;
    double sample_length = dynamic_sample_length;
    double  final_min;
    double  final_max;
    double  final_range;
    double  final_rms;

    int min_data =  8388607;
    int max_data = -8388607;

    int64_t min_sum =  0;
    int64_t max_sum =  0;


    int32_t Value = 0;

    int32_t range = 0;
    AD717X_ComputeDataregSize(ad7175_device);
    for(int i = 0; i < sample_number; i++){
        emit set_progress_bar(i);
        min_data =  8388607;
        max_data = -8388607;
        for(int j = 0 ; j < sample_length ; j++){
            Value = 0;
            /* Read ADC data and write on Value*/

            int ret = Dynamic_Read(ad7175_device, &Value);
            if (ret < 0)
            {
                emit error_occured("Worker Thread: dynamic_test, Dynamic_Read, error code="+QString::number(ret));
                qInfo() << "dynamic_test: read ADC value failed. error code: " << ret;
                j = sample_length;
                i = sample_number;
            }
            if(Value < min_data)
               min_data = Value;
            if(Value > max_data)
                max_data = Value;
        }
        min_sum = min_sum + min_data;
        max_sum = max_sum + max_data;
    }
    emit set_progress_bar(100);
    if(sample_number > 0){
        final_min = min_sum / sample_number;
        final_max = max_sum / sample_number;
        final_min = ADC2DOUBLE_BIPOLAR(final_min) ;
        final_max =  ADC2DOUBLE_BIPOLAR(final_max) ;
        final_range =  final_max - final_min;
        final_rms = final_range*0.7071068;
        QVariantList data;
        data.append(final_min);
        data.append(final_max);
        data.append(final_range);
        data.append(final_rms);
        emit dynamic_test_ready(data);
    }
//    emit timer_start();
//    return range;
 qInfo() << "Final min: "<< final_min << "Final max" << final_max << "range: " << final_range;
}

void Worker::natural_freq_calc()
{
    // read the configuration parameters
    int frequency = 1;

    int start_value = 0;
    int end_value = 0;
//    emit timer_stop();
    emit start_sinusoid_gen(nf_freq_start);
    bcm2835_delay(1);
    start_value= Worker::capture_dynamic();

    emit start_sinusoid_gen(nf_freq_end);
    bcm2835_delay(1);
    end_value = Worker::capture_dynamic();


    frequency = natural_freq_search(nf_freq_start,nf_freq_end,start_value,end_value,0);
    qInfo() << "NAtural Freq: " << frequency;
    emit naturalfreq_ready(frequency);

    if((frequency >= nf_min_acceptable) &&(frequency <= nf_max_acceptable)){
//        ui->nf_pass_lbl->setText("3");
//        ui->nf_pass_lbl->setStyleSheet("background-color: transparent; "
//                                       "color:rgb(85, 170, 127);");
        qInfo() << "Natural Frequncy check passed!";
    }
    else {
//        ui->nf_pass_lbl->setText("6");
//        ui->nf_pass_lbl->setStyleSheet("background-color: transparent; "
//                                       "color:rgb(194, 67, 70);");
        qInfo() << "Natural Frequncy check failed!!";
    }
//    emit timer_start();
}

void Worker::cuttoff_freq_calc()
{
    int frequency = 100;
    int value_100Hz = 0;
    int value_3db = 0;
    int loop_number = 0;
//    emit timer_stop();
     emit start_sinusoid_gen(frequency);
     bcm2835_delay(1);
     value_100Hz= Worker::capture_dynamic();
     value_3db = int(0.707 * value_100Hz);

     frequency = Worker::cutt_off_freq_search(sf_freq_start,sf_freq_end,value_3db,loop_number);
     //
//     ui->sf_lbl->setText(QString::number(frequency));
     emit cutoff_ready(frequency);
     if((frequency >= cf_min_acceptable) &&(frequency <= cf_max_acceptable)){
//         ui->cf_pass_lbl->setText("3");
//         ui->cf_pass_lbl->setStyleSheet("background-color: transparent; "
//                                        "color:rgb(85, 170, 127);");
         qInfo() << "Cutt-off freq check passed!";
//         return true;
     }
     else {
//         ui->cf_pass_lbl->setText("6");
//         ui->cf_pass_lbl->setStyleSheet("background-color: transparent; "
//                                        "color:rgb(194, 67, 70);");
         qInfo() << "Cutt-off freq check failed!";
//         return false;
     }
//     emit timer_start();
}

void Worker::over_shoot_calc()
{
    int value_1Hz = 0;
    int value_100Hz = 0;
    double over_shoot = 0;

    qInfo() << "Over Shoot; 1 Hz Frequency ...";
//    emit timer_stop();
    emit start_square_gen(2);
    bcm2835_delay(1);
    value_1Hz = capture_square();

    qInfo() << "Over Shoot; 100 Hz Frequency ..";

    emit start_square_gen(100);
    bcm2835_delay(1);
    value_100Hz = capture_dynamic();

    if(value_1Hz > 0)
        over_shoot =  (value_100Hz - value_1Hz)*100 /  value_1Hz;
//     ui->os_lbl->setText(QString::number(int(over_shoot)));
    qInfo() << "OverShoot=  " << over_shoot;
    emit overshoot_ready(over_shoot);

     if( (over_shoot <= Os_max_acceptable) && (over_shoot >= 0)){
//         ui->os_pass_lbl->setText("3");
//         ui->os_pass_lbl->setStyleSheet("background-color: transparent; "
//                                        "color:rgb(85, 170, 127);");
         qInfo() << "OverShoot check passed!";
//         return true;
     }
     else {
//         ui->os_pass_lbl->setText("6");
//         ui->os_pass_lbl->setStyleSheet("background-color: transparent; "
//                                        "color:rgb(194, 67, 70);");
         qInfo() << "OverShoot check failed!";
//         return false;
     }
//     emit timer_stop();
}



void Worker::static_real_time()
{
    qInfo() << this << "******Worker: " << QThread::currentThreadId();

    if(m_producer == false) {

        quit();
    }

    int32_t  adc_data = 0;
    int ret = 0;
    ret = Static_Read(ad7175_device, &adc_data);
    if(ret<0) {
        emit error_occured("Worker Thread: static_real_time, Static_Read, error code="+QString(ret));
        qInfo() << "Error reading value\n";
    }
    qInfo() << "ADC RAW DATA= " <<  adc_data;
//    double real_voltage = (adc_data/2) * (2.5/(0.75*8388608)) / 1.333333;
    double real_voltage = (double) ((((double) adc_data) - 8388608)*2.5) /(8388608) ;
    qInfo() << "Real-Time Voltage after conversion: " << real_voltage;
    qInfo() << "Voltage: " << real_voltage;
    emit static_real_time_ready(real_voltage);


}
