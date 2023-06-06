QT += quick
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../ACC-debug/src/ad717x.c \
        ad9833.c \
        bridge.cpp \
        file_manager.cpp \
        main.cpp \
        main_adc.c \
        pigpio.cpp \
        spidev_lib.c \
        src/ad770x.c \
        src/bcm2835.c \
        src/file_gpio.c \
        src/max31865.c \
        worker.cpp

RESOURCES += qml.qrc \
    Images.qrc
#INCLUDEPATH += /home/faryad/projects/ACC/RaspberryDemo/test.h
#QMAKE_CC        = gcc
#COMPILER_FLAGS = -march=armv7-a -mtune=cortex-a8 -mthumb -mfpu=neon -mfloat-abi=soft
#QMAKE_CFLAGS += $${COMPILER_FLAGS}
#QMAKE_CC = /opt/qt5pi/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
#QMAKE_CFLAGS += -std=c11 -Wall -Wformat -Werror -Wshadow
#QMAKE_CFLAGS += $$(CFLAGS)
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =
#LIBS += -L"/home/pi/raspi4/sysroot/usr/lib"
#LIBS += -lwiringPi
#LIBS += -ad717x
# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = sudo /home/pi/$${TARGET}
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ad7175_2_regs.h \
    ad717x.h \
    ad770x.h \
    ad9833.h \
    bcm2835.h \
    bridge.h \
    file_gpio.h \
    file_manager.h \
    main_adc.h \
    max31865.h \
    pigpio.h \
    spidev_lib.h \
    worker.h
# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./source/  # current QXlsx source path is ./source/
include(./QXlsx.pri)
