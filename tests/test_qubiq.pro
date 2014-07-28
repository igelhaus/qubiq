QT += core testlib
QT -= gui

TARGET    = test_qubiq

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += "../include"

HEADERS = ../include/qubiq/lexeme.h
SOURCES = test_lexeme.cpp ../src/lexeme.cpp
