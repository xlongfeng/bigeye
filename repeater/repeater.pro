QT += core
QT -= gui

CONFIG += c++11

TARGET = bigeye-repeater
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    repeater.cpp \
    hidgadget.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    repeater.h \
    hidgadget.h

unix|win32: LIBS += -L$$OUT_PWD/../fishbone/ -lfishbone

INCLUDEPATH += $$PWD/../fishbone
DEPENDPATH += $$PWD/../fishbone

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../fishbone/fishbone.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../fishbone/libfishbone.a

INCLUDEPATH += $$PWD/../qextserialport/src
DEPENDPATH += $$PWD/../qextserialport

greaterThan(QT_MAJOR_VERSION, 4) {
    unix|win32: LIBS += -L$$OUT_PWD/../qextserialport/ -lQt5ExtSerialPort

    win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../qextserialport/Qt5ExtSerialPort.lib
    else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../qextserialport/libQt5ExtSerialPort.a
} else {
    unix|win32: LIBS += -L$$OUT_PWD/../qextserialport/ -lqextserialport

    win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../qextserialport/qextserialport.lib
    else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../qextserialport/libqextserialport.a
}

equals(QMAKE_CXX, "arm-linux-g++") {
    unix:!macx: LIBS += -lvpu -lipu
}
