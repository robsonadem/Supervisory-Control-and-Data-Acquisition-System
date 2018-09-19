TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    dms.cpp \
    table.cpp\
    ../../tools/log.cpp\
    ../../tools/baseclass.cpp \
    dmswd.cpp \
    sqlite3.c \

#LIBS += -l sqlite3

HEADERS += \
    dms.h \
    table.h\
    ../../tools/log.h\
    ../../tools/baseclass.h \
    dmswd.h \
    sqlite3.h
