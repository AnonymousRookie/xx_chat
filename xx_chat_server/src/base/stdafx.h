#ifndef BASE_STDAFX_H
#define BASE_STDAFX_H

#ifdef _WIN32
    #define localtime_r(a, b) localtime_s(b, a)
#endif  // _WIN32

#endif  // BASE_STDAFX_H