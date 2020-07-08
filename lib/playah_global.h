#ifndef PLAYAH_GLOBAL_H
#define PLAYAH_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PLAYAH_LIBRARY)
#  define PLAYAH_EXPORT Q_DECL_EXPORT
#else
#  define PLAYAH_EXPORT Q_DECL_IMPORT
#endif

#endif // PLAYAH_GLOBAL_H
