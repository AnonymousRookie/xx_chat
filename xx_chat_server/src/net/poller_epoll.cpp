#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include "poller_epoll.h"
#include "channel.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

EPollPoller::EPollPoller(EventLoop* loop)
    : Poller(loop)
    , events_(kInitEventListSize)
{
    epollfd_ = ::epoll_create(1024);  // 1024 is just a hint for the kernel
    if (epollfd_ < 0) {
        printf("epoll_create error!\n");
    }
}

EPollPoller::~EPollPoller()
{
    ::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int eventNum = ::epoll_wait(epollfd_, &*events_.begin(), events_.size(), timeoutMs);
    Timestamp now(Timestamp::now());
    if (0 == eventNum)
    {
        // LOG_INFO("poll timeout, nothing happened!");
    }
    else if (eventNum > 0)
    {
        fillActiveChannels(eventNum, activeChannels);
        if (eventNum == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    }
    return now;
}

void EPollPoller::updateChannel(Channel* channel)
{
    Poller::assertInLoopThread();
    const int index = channel->index();
    LOG_INFO("fd = %d, events = %d, index = %d", channel->fd(), channel->events(), index);
    if (index == kNew || index == kDeleted)
    {
        // a new one, add with EPOLL_CTL_ADD
        int fd = channel->fd();
        if (index == kNew)
        {
            assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;
        }
        else // index == kDeleted
        {
            assert(channels_.find(fd) != channels_.end());
            assert(channels_[fd] == channel);
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        // update existing one with EPOLL_CTL_MOD/DEL
        int fd = channel->fd();
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        assert(index == kAdded);
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel* channel)
{
    Poller::assertInLoopThread();
    int fd = channel->fd();
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = channels_.erase(fd);
    (void)n;
    assert(n == 1);

    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

void EPollPoller::fillActiveChannels(int eventNum, ChannelList* activeChannels)
{
    assert(eventNum <= events_.size());
    for (int i = 0; i < eventNum; ++i)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        int fd = channel->fd();
        auto it = channels_.find(fd);
        assert(it != channels_.end());
        assert(it->second == channel);

        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        LOG_ERROR("epoll_ctl op = %s, fd = %d", operationToString(operation), fd);
    }
}

const char* EPollPoller::operationToString(int op)
{
  switch (op)
  {
    case EPOLL_CTL_ADD:
      return "ADD";
    case EPOLL_CTL_DEL:
      return "DEL";
    case EPOLL_CTL_MOD:
      return "MOD";
    default:
      return "Unknown Operation";
  }
}

NAMESPACE_END(net)
NAMESPACE_END(z)
