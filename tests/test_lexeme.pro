#
# Tests for class Lexeme
#

QT += core testlib
QT -= gui

TARGET    = test_lexeme
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE  = app

INCLUDEPATH += "../include"

# NB! Currently project sources are compiled into tests, thus we have to add
# headers explicitly so that MOC could generate its code:
HEADERS = ../include/qubiq/lexeme.h
SOURCES = test_lexeme.cpp ../src/lexeme.cpp
