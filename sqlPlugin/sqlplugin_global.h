#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(SQLPLUGIN_LIB)
#  define SQLPLUGIN_EXPORT Q_DECL_EXPORT
# else
#  define SQLPLUGIN_EXPORT Q_DECL_IMPORT
# endif
#else
# define SQLPLUGIN_EXPORT
#endif
