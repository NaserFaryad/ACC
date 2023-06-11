#include <QQuickView>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QMetaObject>
#include "pigpio.h"
#include "worker.h"
#include "bridge.h"
#include <QThread>
#include "file_manager.h"

int main(int argc, char *argv[])
{


    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // the application class
    QGuiApplication app(argc, argv);

    // Threading
    QThread adcThread;
    Worker adc;

//    QThread singenThread;
    PiGPIO singen;
//    auto singen = new PiGPIO;

    Bridge bridge;
    adc.moveToThread(&adcThread);
//    singen.moveToThread(&singenThread);

//    singenThread.setObjectName("Signal Generator Thread");
//    singen.setObjectName("SINGEN");
//    QObject::connect(&singenThread,&QThread::started,&singen,&PiGPIO::exec, Qt::QueuedConnection);
//    QMetaObject::invokeMethod(&singen, "exec", Qt::QueuedConnection);
    adcThread.setObjectName("Main ADC Thread");
    adc.setObjectName("ADC");

    // Left Column and Calibration Tab Connection
    bool check1 = QObject::connect(&adc,&Worker::adc_ready,&bridge,&Bridge::button_values, Qt::QueuedConnection);
    qInfo() <<"successful connection?: check1= "<<check1;
    bool check2 = QObject::connect(&bridge, SIGNAL(read_static_voltage(QVariant)), &adc, SLOT(read_static_voltage(QVariant)), Qt::QueuedConnection);
    qInfo() <<"successful connection?: check2= "<<check2;

    // Static RealTime connection
    QObject::connect(&bridge,&Bridge::static_real_time_started,&adc,&Worker::static_real_time_start, Qt::QueuedConnection);
    QObject::connect(&adc,&Worker::static_real_time_ready,&bridge,&Bridge::static_real_time_value, Qt::QueuedConnection);
    QObject::connect(&bridge,&Bridge::static_real_time_stoped,&adc,&Worker::static_real_time_stop, Qt::QueuedConnection);

    // SigGen: Dynamic Test Connection
    QObject::connect(&bridge,&Bridge::start_dynamic_test,&adc,&Worker::dynamic_test, Qt::QueuedConnection);
    QObject::connect(&adc,&Worker::start_sinusoid_gen,&singen,&PiGPIO::Sinusoid_Gen, Qt::QueuedConnection);
    QObject::connect(&adc,&Worker::start_square_gen,&singen,&PiGPIO::Square_Gen, Qt::QueuedConnection);
    QObject::connect(&adc,&Worker::dynamic_test_ready,&bridge,&Bridge::dynamic_test_values, Qt::QueuedConnection);
    QObject::connect(&bridge,&Bridge::start_natural_freq_calc,&adc,&Worker::natural_freq_calc, Qt::QueuedConnection);
    QObject::connect(&bridge,&Bridge::start_cuttoff_freq_calc,&adc,&Worker::cuttoff_freq_calc, Qt::QueuedConnection);
    QObject::connect(&bridge,&Bridge::start_over_shoot_calc,&adc,&Worker::over_shoot_calc, Qt::QueuedConnection);
    QObject::connect(&adc,&Worker::overshoot_ready,&bridge,&Bridge::overshoot, Qt::QueuedConnection);
    QObject::connect(&adc,&Worker::cutoff_ready, &bridge,&Bridge::cutoff_slot, Qt::QueuedConnection);
    QObject::connect(&adc,&Worker::naturalfreq_ready, &bridge,&Bridge::naturalfreq, Qt::QueuedConnection);
    QObject::connect(&adc,&Worker::timer_stop,&singen,&PiGPIO::stop, Qt::QueuedConnection);
    QObject::connect(&adc,&Worker::timer_start,&singen,&PiGPIO::timer_start, Qt::QueuedConnection);

    // Temperature and Current sensor connection
    QObject::connect(&singen,&PiGPIO::print_current,&bridge,&Bridge::sensor_current_temp, Qt::QueuedConnection);

    // Time and Date Connection
    QObject::connect(&singen,&PiGPIO::notice,&bridge,&Bridge::time_date, Qt::QueuedConnection);

    // Print error Connection
    QObject::connect(&adc,&Worker::error_occured,&bridge,&Bridge::print_error, Qt::QueuedConnection);
    QObject::connect(&singen,&PiGPIO::error_occured,&bridge,&Bridge::print_error, Qt::QueuedConnection);

    // Progress bar connection
    QObject::connect(&adc,&Worker::set_progress_bar,&bridge,&Bridge::progress_bar_value, Qt::QueuedConnection);

    // Internal Calibration connection
    QObject::connect(&bridge,&Bridge::start_internal_calibration,&adc,&Worker::internal_calibration, Qt::QueuedConnection);

    // the Qml engine
    QQmlApplicationEngine engine;

    // Application title
    QCoreApplication::setApplicationName("ACC Calibration");

    file_manager files;
    engine.rootContext()->setContextProperty("singen", &singen);
    engine.rootContext()->setContextProperty("bridge", &bridge);
    engine.rootContext()->setContextProperty("files", &files);
    // the url of the qml file
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    adcThread.start();
    singen.start();
    return app.exec();
}
