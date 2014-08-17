QT       += core
QT       -= gui
TEMPLATE  = app
TARGET    = extract-terms
CONFIG   -= app_bundle
CONFIG   += console
DESTDIR   = ..

INCLUDEPATH += "../core/include" "../3rdparty" "../3rdparty/cutelogger" "../3rdparty/cutelogger/include"
LIBS        += -L../ -L../3rdparty/cutelogger -lqubiq -lLogger

SOURCES += main.cpp
