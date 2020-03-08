/**
* Copyright 2018-2019, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: NONE
*/

#ifndef BASE_SRC_DEFINES_H
#define BASE_SRC_DEFINES_H

#include <assert.h>

#ifdef _WIN32
#define localtime_r(a, b) localtime_s(b, a)
#endif  // _WIN32

#define NAMESPACE_BEGIN(n) namespace n {
#define NAMESPACE_END(n)   }

#define Z_CHECK(x) if (!(x)) { return; }
#define Z_CHECK_RETURN(x, v) if (!(x)) { return (v); }
#define Z_CHECK_CONTINUE(x) if (!(x)) { continue; }
#define Z_CHECK_BREAK(x) if (!(x)) { break; }

#define Z_CHECK_EQUAL_ASSERT(x, v) assert((v) == (x));
#define Z_CHECK_ASSERT(x) assert((x));


#endif  // BASE_SRC_DEFINES_H