#include <assert.h>
#include "event_loop_thread_pool.h"
#include "event_loop.h"
#include "event_loop_thread.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& name)
    : baseLoop_(baseLoop)
    , name_(name)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

bool EventLoopThreadPool::started()
{
    return started_;
}

const std::string EventLoopThreadPool::name()
{
    return name_;
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    baseLoop_->assertInLoopThread();
    assert(started_);
    EventLoop* loop = baseLoop_;

    if (!loops_.empty())
    {
        loop = loops_[next_];
        ++next_;
        if (next_ >= loops_.size())
        {
            next_ = 0;
        }
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllEventLoops()
{
    baseLoop_->assertInLoopThread();
    assert(started_);
    if (loops_.empty()) {
        return std::vector<EventLoop*>(1, baseLoop_);
    }
    else {
        return loops_;
    }
}

void EventLoopThreadPool::start()
{
    assert(!started_);
    baseLoop_->assertInLoopThread();
    started_ = true;

    for (int i = 0; i < threadNum_; ++i)
    {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);
        EventLoopThread* t = new EventLoopThread(buf);
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());
    }
}

NAMESPACE_END(net)
NAMESPACE_END(z)