QT       += core
QT       -= gui
TEMPLATE  = app
TARGET    = build-transducer
CONFIG   -= app_bundle
CONFIG   += console
DESTDIR   = ..

INCLUDEPATH += "../util/include" "../3rdparty" "../3rdparty/cutelogger" "../3rdparty/cutelogger/include"
LIBS        += -L../ -L../3rdparty/cutelogger -lqubiqutil -lLogger

HEADERS += main.h

SOURCES += main.cpp
