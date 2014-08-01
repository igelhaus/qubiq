#-------------------------------------------------
#
# Project created by QtCreator 2014-03-05T09:10:37
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET    = qubiq

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += "include"
HEADERS     += \
    include/qubiq/extractor.h       \
    include/qubiq/lexeme.h          \
    include/qubiq/lexeme_sequence.h \
    include/qubiq/text.h

SOURCES += main.cpp          \
    src/extractor.cpp        \
    src/lexeme.cpp           \
    src/lexeme_sequence.cpp  \
    src/text.cpp

