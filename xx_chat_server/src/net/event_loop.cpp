#include <sstream>
#include <algorithm>
#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include "event_loop.h"
#include "sockets_ops.h"
#include "poller_epoll.h"
#include "channel.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

thread_local EventLoop* t_loopInThisThread = 0;

EventLoop* getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

EventLoop::EventLoop()
{
    if (t_loopInThisThread) {
        printf("Another EventLoop exists in this thread!\n");
    }
    else {
        t_loopInThisThread = this;
    }

    wakeupFd_ = createWakeupfd();
    poller_.reset(new EPollPoller(this));
    wakeupChannel_.reset(new Channel(this, wakeupFd_));

    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;

    while (!quit_)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);

        eventHandling_ = true;
        for (Channel* channel : activeChannels_)
        {
            currentActiveChannel_ = channel;
            currentActiveChannel_->handleEvent(pollReturnTime_);
        }
        currentActiveChannel_ = nullptr;
        eventHandling_ = false;

        doPendingFunctors();
    }
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread()) {
        cb();
    }
    else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.push_back(cb);
    }
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

size_t EventLoop::queueSize()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return pendingFunctors_.size();
}

void EventLoop::updateChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (eventHandling_)
    {
        assert(currentActiveChannel_ == channel ||
            std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
    }
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    return poller_->hasChannel(channel);
}

void EventLoop::assertInLoopThread()
{
    if (!isInLoopThread())
    {
        abortNotInLoopThread();
    }
}

bool EventLoop::isInLoopThread()
{ 
    return threadId_ == std::this_thread::get_id();     
}

void EventLoop::abortNotInLoopThread()
{
    std::ostringstream oss;
    oss << "this created in thread id: " << threadId_
        << ", current thread id: " << std::this_thread::get_id();
    printf("%s\n", oss.str().c_str());
    abort();
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor& functor : functors)
    {
        functor();
    }

    callingPendingFunctors_ = false;
}

int EventLoop::createWakeupfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_ERROR("Failed in eventfd");
        abort();
    }
    return evtfd;
}

void EventLoop::wakeup()
{
  uint64_t one = 1;
  sockets::write(wakeupFd_, &one, sizeof one);
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = sockets::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR("EventLoop::handleRead() reads %d bytes instead of 8", n);
    }
}

NAMESPACE_END(net)
NAMESPACE_END(z)