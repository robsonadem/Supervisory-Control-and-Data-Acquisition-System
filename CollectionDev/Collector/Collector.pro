TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
           sqlite3.c \

HEADERS += sqlite3.h

INCLUDEPATH += "C:/Program Files/Phidgets/Phidget22"
LIBS += -L "C:/Program Files/Phidgets/Phidget22/x86" -lphidget22

## macOS
#unix:{
#SOURCES += main.c \

#LIBS += -l sqlite3
