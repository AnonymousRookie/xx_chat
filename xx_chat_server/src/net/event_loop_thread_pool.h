#ifndef Z_NET_SRC_EVENT_LOOP_THREAD_POOL_H
#define Z_NET_SRC_EVENT_LOOP_THREAD_POOL_H

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include "defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
    EventLoopThreadPool(EventLoop* baseLoop, const std::string& name);
    ~EventLoopThreadPool();

    void setThreadNum(int num) { threadNum_ = num; }
    void start();

    bool started();
    const std::string name();

    EventLoop* getNextLoop();
    std::vector<EventLoop*> getAllEventLoops();

private:
    EventLoop* baseLoop_ = nullptr;
    std::string name_;
    bool started_ = false;
    int threadNum_ = 0;
    int next_ = 0;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_EVENT_LOOP_THREAD_POOL_H