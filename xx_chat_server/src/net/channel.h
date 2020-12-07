#ifndef Z_NET_SRC_CHANNEL_H
#define Z_NET_SRC_CHANNEL_H

#include <functional>
#include <memory>
#include <string>
#include <stdint.h>
#include <assert.h>
#include "defines.h"
#include "time_stamp.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

class EventLoop;

class Channel
{
public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void(Timestamp)> ReadEventCallback;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(Timestamp receiveTime);
    void setReadCallback(ReadEventCallback cb);
    void setWriteCallback(EventCallback cb);
    void setCloseCallback(EventCallback cb);
    void setErrorCallback(EventCallback cb);

    void tie(const std::shared_ptr<void>&);

    int fd();

    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; } 

    bool isNoneEvent() const { return events_ == kNoneEvent; }

    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

    std::string reventsToString() const;
    std::string eventsToString() const;

    EventLoop* ownerLoop() { return loop_; }
    void remove();

private:
    static std::string eventsToString(int fd, int ev);

    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    int fd_ = -1;

    int events_ = 0;
    int revents_ = 0;
    int index_ = -1;

    std::weak_ptr<void> tie_;
    bool tied_ = false;

    bool eventHandling_ = false;
    bool addedToLoop_ = false;
    
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // Z_NET_SRC_CHANNEL_H