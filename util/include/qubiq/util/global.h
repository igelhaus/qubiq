#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <QtCore>

#if defined(QUBIQUTIL_LIBRARY)
#  define QUBIQUTILSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QUBIQUTILSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // _GLOBAL_H_
