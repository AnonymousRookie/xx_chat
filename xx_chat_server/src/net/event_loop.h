#ifndef Z_NET_SRC_EVENT_LOOP_H
#define Z_NET_SRC_EVENT_LOOP_H

#include <string>
#include <cstdint>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "defines.h"
#include "time_stamp.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class Channel;
class Poller;

class EventLoop
{
public:
    typedef std::function<void()> Functor;
    
    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);

    size_t queueSize();

    void wakeup();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    void assertInLoopThread();

    bool isInLoopThread();

private:
    void abortNotInLoopThread();
    void doPendingFunctors();
    int createWakeupfd();
    void handleRead();

private:
    typedef std::vector<Channel*> ChannelList;

    bool looping_ = false;
    bool quit_ = false;
    bool eventHandling_ = false;
    bool callingPendingFunctors_ = false; 

    const std::thread::id threadId_ = std::this_thread::get_id();
    const int kPollTimeMs = 100;

    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_ = nullptr;

    int wakeupFd_ = -1;
    std::unique_ptr<Channel> wakeupChannel_;

    std::mutex mutex_;
	std::vector<Functor> pendingFunctors_;
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_EVENT_LOOP_H