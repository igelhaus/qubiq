#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <QtCore>

#if defined(QUBIQ_LIBRARY)
#  define QUBIQSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QUBIQSHARED_EXPORT Q_DECL_IMPORT
#endif

inline char* toLoggable(const QString &s)    { return s.toUtf8().data(); }
inline char* toLoggable(const QStringRef &s) { return s.toUtf8().data(); }

#endif // _GLOBAL_H_
