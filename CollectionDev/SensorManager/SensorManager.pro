TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    sensormanager.cpp \
    sensor.cpp

# include library for lphidget22
INCLUDEPATH += "C:/Program Files/Phidgets/Phidget22"
LIBS += -L "C:/Program Files/Phidgets/Phidget22/x86" -lphidget22

# include library

HEADERS += \
    sensormanager.h \
    sensor.h
