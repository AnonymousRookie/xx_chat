#ifndef MSG_SERVER_STDAFX_H
#define MSG_SERVER_STDAFX_H

#include "logger.h"

#ifdef _WIN32
    #define localtime_r(a, b) localtime_s(b, a)
#endif  // _WIN32

#endif  // MSG_SERVER_STDAFX_H