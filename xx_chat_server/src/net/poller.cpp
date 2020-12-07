#include "poller.h"
#include "channel.h"
#include "event_loop.h"
#include "logger.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

Poller::Poller(EventLoop* loop)
    : ownerLoop_(loop)
{

}

bool Poller::hasChannel(Channel* channel)
{
    assertInLoopThread();
    auto it = channels_.find(channel->fd());
    return (it != channels_.end() && it->second == channel);
}

void Poller::assertInLoopThread()
{
    ownerLoop_->assertInLoopThread();
}

NAMESPACE_END(net)
NAMESPACE_END(z)