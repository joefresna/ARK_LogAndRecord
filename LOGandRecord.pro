#-------------------------------------------------
#
# Project created by QtCreator 2017-02-08T11:35:48
#
#-------------------------------------------------

QT       -= gui

QT += widgets

TARGET = LOGandRecordExp
TEMPLATE = lib

DEFINES += LOGandRecordEXP_LIBRARY

SOURCES += \
    kilobot.cpp \
    LOGandRecordEnv.cpp \
    LOGandRecordExp.cpp

HEADERS +=\
    kilobot.h \
    kilobotexperiment.h \
    kilobotenvironment.h \
    LOGandRecordExp.h \
    LOGandRecordEnv.h \
    global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += /usr/local/include/
LIBS += -L/usr/local/lib \
        -lopencv_core
