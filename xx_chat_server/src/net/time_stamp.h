#ifndef Z_NET_SRC_TIME_STAMP_H
#define Z_NET_SRC_TIME_STAMP_H

#include <map>
#include <vector>
#include <stdint.h>
#include "defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch);

    ~Timestamp();

    static Timestamp now();

private:
    int64_t microSecondsSinceEpoch_;
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_TIME_STAMP_H