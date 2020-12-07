#ifndef Z_NET_SRC_POLLER_H
#define Z_NET_SRC_POLLER_H

#include <map>
#include <vector>
#include "defines.h"
#include "time_stamp.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class Channel;
class EventLoop;

class Poller 
{
public:
    typedef std::vector<Channel*> ChannelList;
    typedef std::map<int, Channel*> ChannelMap;

    Poller(EventLoop* loop);
    virtual ~Poller() = default;

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
    virtual bool hasChannel(Channel* channel);

    void assertInLoopThread();
    
protected:
    ChannelMap channels_;

private:
    EventLoop* ownerLoop_;
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_POLLER_H