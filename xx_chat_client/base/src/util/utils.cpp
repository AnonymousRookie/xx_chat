/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: utils
 */

#include "utils.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(utils)

std::mt19937& RandomUtil::GetEngine() {
    static std::random_device seed_gen;
    static std::mt19937 engine(seed_gen());
    return engine;
}

uint64_t GetTickCount()
{
#ifdef _WIN32
    LARGE_INTEGER liCounter; 
    LARGE_INTEGER liCurrent;

    if (!QueryPerformanceFrequency(&liCounter))
        return ::GetTickCount();
    QueryPerformanceCounter(&liCurrent);
    return (uint64_t)(liCurrent.QuadPart * 1000 / liCounter.QuadPart);
#else
    struct timeval tval;
    uint64_t tick;
    gettimeofday(&tval, NULL);
    tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
    return tick;
#endif
}

NAMESPACE_END(utils)
NAMESPACE_END(z)