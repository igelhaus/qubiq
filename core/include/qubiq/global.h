#ifndef _QUBIQ_GLOBAL_H_
#define _QUBIQ_GLOBAL_H_

#include <QtCore>

#if defined(QUBIQ_LIBRARY)
#  define QUBIQSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QUBIQSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // _QUBIQ_GLOBAL_H_
