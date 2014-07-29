#
# Tests for class Text
#

QT += core testlib
QT -= gui

TARGET    = test_qubiq
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE  = app

INCLUDEPATH += "../include"

# NB! Currently project sources are compiled into tests, thus we have to add
# headers explicitly so that MOC could generate its code:
HEADERS = ../include/qubiq/lexeme.h ../include/qubiq/text.h
SOURCES = test_text.cpp ../src/lexeme.cpp ../src/text.cpp
