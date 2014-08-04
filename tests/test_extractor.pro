#
# Tests for class Extractor
#

QT += core testlib
QT -= gui

TARGET    = test_extractor
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE  = app

INCLUDEPATH += "../include"

# NB! Currently project sources are compiled into tests, thus we have to add
# headers explicitly so that MOC could generate its code:
HEADERS = ../include/qubiq/lexeme.h \
    ../include/qubiq/text.h \
    ../include/qubiq/lexeme_sequence.h \
    ../include/qubiq/extractor.h

SOURCES = test_extractor.cpp \
    ../src/lexeme.cpp \
    ../src/text.cpp \
    ../src/lexeme_sequence.cpp \
    ../src/extractor.cpp
