#ifndef LOGIN_SERVERR_STDAFX_H
#define LOGIN_SERVERR_STDAFX_H

#include "logger.h"

#ifdef _WIN32
    #define localtime_r(a, b) localtime_s(b, a)
#endif  // _WIN32

#endif  // LOGIN_SERVERR_STDAFX_H