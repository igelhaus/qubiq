#-------------------------------------------------
#
# Project created by QtCreator 2015-07-03T08:13:30
#
#-------------------------------------------------

QT       += core
QT       -= gui
TARGET    = qubiqutil
DEFINES  += QUBIQUTIL_LIBRARY
TEMPLATE  = lib
LIBS     += -L../3rdparty/cutelogger -lLogger # FIXME: depends on logger?

DESTDIR   = ..

INCLUDEPATH += "include" "../3rdparty"
HEADERS     += \
    include/qubiq/util/global.h       \
    include/qubiq/util/lexeme.h       \
    include/qubiq/util/lexeme_index.h

SOURCES += \
    src/lexeme.cpp       \
    src/lexeme_index.cpp 

mac {
    # Copy 3rd party libs to common dest dir to run tests:
    QMAKE_POST_LINK += cp -R $$[OUT_PWD]../3rdparty/cutelogger/libLogger.* $$DESTDIR
}
