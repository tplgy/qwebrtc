#ifndef QWEBRTC_GLOBAL_H
#define QWEBRTC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QWEBRTC_LIBRARY)
#  define QWEBRTCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QWEBRTCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QWEBRTC_GLOBAL_H
