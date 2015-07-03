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
LIBS     += -L.. -lqubiqutil -L../3rdparty/cutelogger -lLogger

DESTDIR   = ..

INCLUDEPATH += "include" "../util/include" "../3rdparty"
HEADERS     += \
    include/qubiq/abstract_term_filter.h  \
    include/qubiq/qubiq_global.h          \
    include/qubiq/extractor.h             \
    include/qubiq/lexeme_sequence.h       \
    include/qubiq/text.h                  \
    include/qubiq/master_lemmatizer.h     \
    include/qubiq/lemmatizer.h            \
    include/qubiq/lemmatizer_interfaces.h

SOURCES += \
    src/extractor.cpp         \
    src/lexeme_sequence.cpp   \
    src/text.cpp              \
    src/master_lemmatizer.cpp

mac {
    # Copy 3rd party libs to common dest dir to run tests:
    QMAKE_POST_LINK += cp -R $$[OUT_PWD]../3rdparty/cutelogger/libLogger.* $$DESTDIR
}
