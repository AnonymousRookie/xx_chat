/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: EventDispatch
 */

#ifndef BASE_EVENT_DISPATCH_H
#define BASE_EVENT_DISPATCH_H

#include <list>
#include <mutex>
#include <memory>
#include <thread>
#include <functional>
#include "types.h"

enum {
	SOCKET_READ	= 0x1,
	SOCKET_WRITE = 0x2,
	SOCKET_EXCEP = 0x4,
	SOCKET_ALL = 0x7
};

class EventDispatch
{
public:
    virtual ~EventDispatch();

    void AddEvent(SOCKET fd, uint8_t socketEvent);
	void RemoveEvent(SOCKET fd, uint8_t socketEvent);

	void AddTimer(const TimerCallback& callback, uint64_t interval);
	void RemoveTimer(const TimerCallback& callback);
    
    void AddLoop(const LoopCallback& callback);

	void StartDispatch(uint32_t waitTimeout = 100);  // 单位: 毫秒
    void StartDispatchInThread();
    void StopDispatch();
    void StopDispatchInThread();
    
    bool IsRunning() { return running_; }

    static EventDispatch* Instance();

private:
    EventDispatch();

    void CheckTimer();
    void CheckLoop();

    struct TimerItem {
        TimerCallback callback;
        uint64_t interval;
        uint64_t nextTick;
    };

    struct LoopItem {
        LoopCallback callback;
    };

private:
    #ifdef _WIN32
        fd_set readSet_;
        fd_set writeSet_;
        fd_set exceptSet_;
    #else
        int epollfd_;
    #endif

    std::list<std::shared_ptr<TimerItem>> timerList_;
    std::list<std::shared_ptr<LoopItem>> loopList_;

    static EventDispatch* eventDispatch_;
    bool running_;

    std::mutex mutex_;
    std::thread eventThread_;
};

#endif  // BASE_EVENT_DISPATCH_H