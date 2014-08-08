QT          += core testlib
QT          -= gui
CONFIG      += console
CONFIG      -= app_bundle
TEMPLATE     = app
DESTDIR      = ../../
INCLUDEPATH += "../../core/include" "../../3rdparty"
LIBS        += -L../../ -lqubiq
