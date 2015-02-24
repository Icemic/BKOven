#ifndef BKAUDIO_GLOBAL_H
#define BKAUDIO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BKAUDIO_LIBRARY)
#  define BKAUDIOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BKAUDIOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // BKAUDIO_GLOBAL_H
