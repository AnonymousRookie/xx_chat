/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: utils
 */

#ifndef BASE_UTILS_H
#define BASE_UTILS_H

#include <iostream>
#include <random>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include "types.h"
#include "defines.h"

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(utils)

/** 
 * 随机数生成器
 */
class RandomUtil 
{
public:
    template<typename T>
    static inline T RandomReal(T min, T max) {
        std::uniform_real_distribution<T> dist(min, max);
        auto& mt = RandomUtil::GetEngine();
        return dist(mt);
    }

    template<typename T>
    static inline T RandomInt(T min, T max) {
        std::uniform_int_distribution<T> dist(min, max);
        auto& mt = RandomUtil::GetEngine();
        return dist(mt);
    }
private:
    static std::mt19937& GetEngine();
};

/**
 * Returns a random value between `min` and `max`.
 */
template<typename T>
inline T Random(T min, T max) {
    return RandomUtil::RandomInt<T>(min, max);
}

template<>
inline float Random(float min, float max) {
    return RandomUtil::RandomReal(min, max);
}

template<>
inline long double Random(long double min, long double max) {
    return RandomUtil::RandomReal(min, max);
}

template<>
inline double Random(double min, double max) {
    return RandomUtil::RandomReal(min, max);
}

uint64_t GetTickCount();

// Tokenizes string. Returns all non-empty substrings
template<typename String, typename Delim, typename Container = std::vector<String>>
Container Strtok(const String & s, const Delim & delims)
{
    Container ret;

    typename String::size_type start{}, pos{};
    do {
        pos = s.find_first_of(delims, start);

        // Not found, we're at ends;
        if (pos == String::npos) {
            if (start < s.size()) {
                ret.emplace_back(s.substr(start));
            }
        }
        else if (pos > start) {
            // Non-empty substring
            ret.emplace_back(s.substr(start, pos - start));
        }
        start = pos + 1;
    } while (pos != String::npos);

    return ret;
}

NAMESPACE_END(utils)
NAMESPACE_END(z)

#endif  // BASE_UTILS_H