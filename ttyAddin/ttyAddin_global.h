#ifndef TTYADDIN_GLOBAL_H
#define TTYADDIN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TTYADDIN_LIBRARY)
#  define TTYADDIN_EXPORT Q_DECL_EXPORT
#else
#  define TTYADDIN_EXPORT Q_DECL_IMPORT
#endif

#endif // TTYADDIN_GLOBAL_H
