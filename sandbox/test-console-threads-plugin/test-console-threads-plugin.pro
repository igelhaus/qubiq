#-------------------------------------------------
#
# Project created by QtCreator 2015-04-25T13:34:49
#
#-------------------------------------------------

QT       += core network
QT       -= gui

TARGET   = test-console-threads-plugin
TEMPLATE = lib

SOURCES += myworkerfactory.cpp \
    myworker.cpp

HEADERS += test-console-threads-plugin_global.h \
    myworkerfactory.h \
    myworker.h

HEADERS += ../test-console-threads/worker.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
