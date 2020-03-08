#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(BASE_LIB)
#  define BASE_EXPORT Q_DECL_EXPORT
# else
#  define BASE_EXPORT Q_DECL_IMPORT
# endif
#else
# define BASE_EXPORT
#endif
