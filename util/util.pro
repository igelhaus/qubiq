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

DESTDIR   = ..

INCLUDEPATH += "include" "../3rdparty"
HEADERS     += \
    include/qubiq/util/qubiqutil_global.h \
    include/qubiq/util/lexeme.h           \
    include/qubiq/util/lexeme_index.h     \
    include/qubiq/util/transition.h       \
    include/qubiq/util/state.h            \
    include/qubiq/util/transducer.h

SOURCES += \
    src/lexeme.cpp       \
    src/lexeme_index.cpp \
    src/transition.cpp   \
    src/state.cpp        \
    src/transducer.cpp
