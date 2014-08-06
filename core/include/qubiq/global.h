#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <QtCore/qglobal.h>

#if defined(QUBIQ_LIBRARY)
#  define QUBIQSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QUBIQSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // _GLOBAL_H_
