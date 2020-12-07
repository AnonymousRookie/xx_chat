#include "event_loop_thread.h"
#include "event_loop.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

EventLoopThread::EventLoopThread(const std::string& name)
{

}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if (loop_) 
    {
        loop_->quit();
        thread_->join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    thread_.reset(new std::thread(std::bind(&EventLoopThread::threadFunc, this)));

    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(loop_ == nullptr) {
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_all();
    }

    loop.loop();
    loop_ = nullptr;
}

NAMESPACE_END(net)
NAMESPACE_END(z)