#ifndef _QUBIQUTIL_GLOBAL_H_
#define _QUBIQUTIL_GLOBAL_H_

#include <QtCore>

#if defined(QUBIQUTIL_LIBRARY)
#  define QUBIQUTILSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QUBIQUTILSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // _QUBIQUTIL_GLOBAL_H_
