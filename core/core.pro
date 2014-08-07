#-------------------------------------------------
#
# Project created by QtCreator 2014-03-05T09:10:37
#
#-------------------------------------------------

QT       += core
QT       -= gui
TARGET    = qubiq
DEFINES  += QUBIQ_LIBRARY
TEMPLATE  = lib
LIBS     += -L../3rdparty/cutelogger -lLogger

INCLUDEPATH += "include" "../3rdparty"
HEADERS     += \
    include/qubiq/global.h          \
    include/qubiq/extractor.h       \
    include/qubiq/lexeme.h          \
    include/qubiq/lexeme_sequence.h \
    include/qubiq/text.h

SOURCES += \
    src/extractor.cpp        \
    src/lexeme.cpp           \
    src/lexeme_sequence.cpp  \
    src/text.cpp
