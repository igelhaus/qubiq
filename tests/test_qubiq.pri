QT          += core testlib
QT          -= gui
CONFIG      += console
CONFIG      -= app_bundle
TEMPLATE     = app
DESTDIR      = ../../
INCLUDEPATH += "../../core/include" "../../util/include" "../../3rdparty"
LIBS        += -L../../ -lqubiqutil -lqubiq
