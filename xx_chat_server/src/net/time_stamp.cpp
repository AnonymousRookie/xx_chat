#include <chrono>
#include <ctime>
#include "time_stamp.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

Timestamp::Timestamp()
    : microSecondsSinceEpoch_(0)
{

}

Timestamp::Timestamp(int64_t microSecondsSinceEpoch)
    : microSecondsSinceEpoch_(microSecondsSinceEpoch)
{

}

Timestamp::~Timestamp()
{

}

Timestamp Timestamp::now()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> now = std::chrono::time_point_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now());

	int64_t microSeconds = now.time_since_epoch().count();
	Timestamp time(microSeconds);
	return time;
}

NAMESPACE_END(net)
NAMESPACE_END(z)