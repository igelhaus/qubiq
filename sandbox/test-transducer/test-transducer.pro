#-------------------------------------------------
#
# Project created by QtCreator 2015-04-22T23:59:07
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET    = test-transducer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    transition.cpp \
    state.cpp \
    transducer.cpp \
    transducer_manager.cpp

HEADERS += \
    transition.h \
    state.h \
    transducer.h \
    transducer_manager.h
