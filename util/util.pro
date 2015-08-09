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
    include/qubiq/util/qubiqutil_global.h   \
    include/qubiq/util/lexeme.h             \
    include/qubiq/util/lexeme_index.h       \
    include/qubiq/util/transducer.h         \
    include/qubiq/util/transducer_manager.h \
    include/qubiq/util/transducer_state.h   \
    include/qubiq/util/transducer_state_transition.h

SOURCES += \
    src/lexeme.cpp             \
    src/lexeme_index.cpp       \
    src/transducer.cpp         \
    src/transducer_manager.cpp \
    src/transducer_state.cpp   \
    src/transducer_state_transition.cpp
