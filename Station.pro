#-------------------------------------------------
# Handler of CanSat data packets
# Created Anatoly Antonov ( https://github.com/champi2061 ) for 8-th Russian Championship of project "Air and Engineering School"
# 2019
#-------------------------------------------------

QT       += core gui serialport charts opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = Station
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
        RC_FILE += rc.rc
        OTHER_FILES += rc.rc
}

