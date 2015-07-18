#-------------------------------------------------
#
# Project created by QtCreator 2015-04-22T23:59:07
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET    = test-console-threads
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    state.cpp \
    transducer.cpp

HEADERS += \
    state.h \
    transducer.h
