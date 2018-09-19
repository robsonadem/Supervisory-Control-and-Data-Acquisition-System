TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    dms.cpp \
    table.cpp\
    ../tools/log.cpp\
    ../tools/baseclass.cpp \
    dmswd.cpp \
#    sqlite3.c \
    ../scada/ControlSubSystem/branch.cpp \
    ../scada/ControlSubSystem/modemanager.cpp \
    ../scada/ControlSubSystem/state.cpp

LIBS += -l sqlite3

HEADERS += \
    dms.h \
    table.h\
    ../tools/log.h\
    ../tools/baseclass.h \
    dmswd.h \
    sqlite3.h \
    ../scada/ControlSubSystem/branch.h \
    ../scada/ControlSubSystem/modemanager.h \
    ../scada/ControlSubSystem/state.h
