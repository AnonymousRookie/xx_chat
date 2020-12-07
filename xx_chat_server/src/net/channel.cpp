#include <sstream>
#include <poll.h>
#include "channel.h"
#include "event_loop.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;  // 1 | 2 = 3
const int Channel::kWriteEvent = POLLOUT;          // 4

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop)
    , fd_(fd)
{

}

Channel::~Channel()
{
    assert(!eventHandling_);
    assert(!addedToLoop_);
    if (loop_->isInLoopThread())
    {
        assert(!loop_->hasChannel(this));
    }
}

void Channel::tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    tied_ = true;
}

void Channel::update()
{
    addedToLoop_ = true;
    loop_->updateChannel(this);
}

void Channel::remove()
{
    assert(isNoneEvent());
    addedToLoop_ = false;
    loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
    std::shared_ptr<void> guard;
    if (tied_)
    {
        guard = tie_.lock();
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::setReadCallback(ReadEventCallback cb)
{
    readCallback_ = cb;
}

void Channel::setWriteCallback(EventCallback cb)
{
    writeCallback_ = cb;
}

void Channel::setCloseCallback(EventCallback cb)
{
    closeCallback_ = cb;
}

void Channel::setErrorCallback(EventCallback cb)
{
    errorCallback_ = cb;
}

int Channel::fd()
{
    return fd_;
}

void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    eventHandling_ = true;
    LOG_INFO("revents = %s", reventsToString().c_str());

    if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        if (closeCallback_) {
            closeCallback_();
        }
    }

    if (revents_ & (POLLERR | POLLNVAL))
    {
        if (errorCallback_) errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (readCallback_) readCallback_(receiveTime);
    }
    if (revents_ & POLLOUT)
    {
        if (writeCallback_) writeCallback_();
    }
    eventHandling_ = false;
}

std::string Channel::reventsToString() const
{
    return eventsToString(fd_, revents_);
}

std::string Channel::eventsToString() const
{
    return eventsToString(fd_, events_);
}

std::string Channel::eventsToString(int fd, int ev)
{
    std::ostringstream oss;
    oss << fd << ": ";
    if (ev & POLLIN)
        oss << "IN ";
    if (ev & POLLPRI)
        oss << "PRI ";
    if (ev & POLLOUT)
        oss << "OUT ";
    if (ev & POLLHUP)
        oss << "HUP ";
    if (ev & POLLRDHUP)
        oss << "RDHUP ";
    if (ev & POLLERR)
        oss << "ERR ";
    if (ev & POLLNVAL)
        oss << "NVAL ";

    return oss.str();
}

NAMESPACE_END(net)
NAMESPACE_END(z)