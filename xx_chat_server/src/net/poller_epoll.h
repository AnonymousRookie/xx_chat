#ifndef Z_NET_SRC_POLLER_EPOLL_H
#define Z_NET_SRC_POLLER_EPOLL_H

#include "defines.h"
#include "poller.h"

struct epoll_event;

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop* loop);
    ~EPollPoller();

    Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;

private:
    const int kNew = -1;
    const int kAdded = 1;
    const int kDeleted = 2;

    static const int kInitEventListSize = 16;   
    void fillActiveChannels(int eventNum, ChannelList* activeChannels);
    void update(int operation, Channel* channel);
    static const char* operationToString(int op);

    typedef std::vector<struct epoll_event> EventList;

    int epollfd_ = -1;
    EventList events_;
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_POLLER_EPOLL_H