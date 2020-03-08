/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: EventDispatch
 */

#include "event_dispatch.h"
#include "base_socket.h"
#include "utils.h"

#define MIN_TIMER_DURATION	100	 // 100 milliseconds

EventDispatch* EventDispatch::eventDispatch_ = NULL;

EventDispatch::EventDispatch() : running_(false)
{
#ifdef _WIN32
	FD_ZERO(&readSet_);
	FD_ZERO(&writeSet_);
	FD_ZERO(&exceptSet_);
#else
	epollfd_ = epoll_create(1024);
	if (-1 == epollfd_) {
		printf("epoll_create failed!\n");
	}
#endif
}

EventDispatch::~EventDispatch()
{
#ifdef _WIN32
    // do nothing
#else
	close(epollfd_);
#endif
}

void EventDispatch::AddTimer(const TimerCallback& callback, uint64_t interval)
{
    /*auto it = timerList_.begin();
    for (; it != timerList_.end(); it++) {
        auto timerItem = *it;
        if (timerItem->callback == callback) {
            timerItem->interval = interval;
            timerItem->nextTick = z::utils::GetTickCount() + interval;
            return;
        }
    }*/
    std::shared_ptr<TimerItem> timerItem = std::make_shared<TimerItem>();
    timerItem->callback = callback;
    timerItem->interval = interval;
    timerItem->nextTick = z::utils::GetTickCount() + interval;
	timerList_.push_back(timerItem);
}

void EventDispatch::RemoveTimer(const TimerCallback& callback)
{
    /*auto it = timerList_.begin();
    for (; it != timerList_.end(); it++) {
        auto timerItem = *it;
        if (timerItem->callback == callback) {
            timerList_.erase(it);
            return;
        }
    }*/
}

void EventDispatch::CheckTimer()
{
	uint64_t currTick = z::utils::GetTickCount();
	auto it = timerList_.begin();
	for (; it != timerList_.end();) {
		auto timerItem = *it;
		it++;  // iterator maybe deleted in the callback, so we should increment it before callback
		if (currTick >= timerItem->nextTick) {
            timerItem->nextTick += timerItem->interval;
            timerItem->callback(NETLIB_MSG_TIMER, 0);
		}
	}
}

void EventDispatch::AddLoop(const LoopCallback& callback)
{
    std::shared_ptr<LoopItem> loopItem = std::make_shared<LoopItem>();
    loopItem->callback = callback;
    loopList_.push_back(loopItem);
}

void EventDispatch::CheckLoop()
{
    auto it = loopList_.begin();
    for (; it != loopList_.end(); ++it) {
        auto loopItem = *it;
        loopItem->callback(NETLIB_MSG_LOOP, 0);
    }
}

EventDispatch* EventDispatch::Instance()
{
	if (NULL == eventDispatch_) {
        eventDispatch_ = new EventDispatch();
	}
	return eventDispatch_;
}

#ifdef _WIN32

void EventDispatch::AddEvent(SOCKET fd, uint8_t socketEvent)
{
	std::lock_guard<std::mutex> lock(mutex_);

	if ((socketEvent & SOCKET_READ) != 0) {
		FD_SET(fd, &readSet_);
	}
	if ((socketEvent & SOCKET_WRITE) != 0) {
		FD_SET(fd, &writeSet_);
	}
	if ((socketEvent & SOCKET_EXCEP) != 0) {
		FD_SET(fd, &exceptSet_);
	}
}

void EventDispatch::RemoveEvent(SOCKET fd, uint8_t socketEvent)
{
	std::lock_guard<std::mutex> lock(mutex_);

	if ((socketEvent & SOCKET_READ) != 0) {
		FD_CLR(fd, &readSet_);
	}
	if ((socketEvent & SOCKET_WRITE) != 0) {
		FD_CLR(fd, &writeSet_);
	}
	if ((socketEvent & SOCKET_EXCEP) != 0) {
		FD_CLR(fd, &exceptSet_);
	}
}

void EventDispatch::StartDispatch(uint32_t waitTimeout/* = 100*/)
{
	fd_set readSet, writeSet, exceptSet;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = waitTimeout * 1000;

    if (running_) {
        return;
    }

    running_ = true;
    
    while (running_) {

		CheckTimer();
        CheckLoop();

		if (!readSet_.fd_count && !writeSet_.fd_count && !exceptSet_.fd_count) {
			Sleep(MIN_TIMER_DURATION);
			continue;
		}

        {
            std::lock_guard<std::mutex> lock(mutex_);
            memcpy(&readSet, &readSet_, sizeof(fd_set));
            memcpy(&writeSet, &writeSet_, sizeof(fd_set));
            memcpy(&exceptSet, &exceptSet_, sizeof(fd_set));
        }

		int nfds = select(0, &readSet, &writeSet, &exceptSet, &timeout);

		if (SOCKET_ERROR == nfds) {
			printf("select failed, error code: %d\n", GetLastError());
			Sleep(MIN_TIMER_DURATION);
			continue;
		}

		if (0 == nfds) {
			continue;
		}

		for (auto i = 0; i < readSet.fd_count; ++i) {
			SOCKET fd = readSet.fd_array[i];
            BaseSocketPtr socket = FindBaseSocket(static_cast<net_handle_t>(fd));
			if (socket) {
				socket->OnRead();
			}
		}

		for (auto i = 0; i < writeSet.fd_count; ++i) {
			SOCKET fd = writeSet.fd_array[i];
            BaseSocketPtr socket = FindBaseSocket(static_cast<net_handle_t>(fd));
			if (socket) {
				socket->OnWrite();
			}
		}

		for (auto i = 0; i < exceptSet.fd_count; ++i) {
			SOCKET fd = exceptSet.fd_array[i];
            BaseSocketPtr socket = FindBaseSocket(static_cast<net_handle_t>(fd));
			if (socket) {
				socket->OnClose();
			}
		}
	}
}

void EventDispatch::StartDispatchInThread()
{
    eventThread_ = std::thread(&EventDispatch::StartDispatch, this, (uint32_t)100);
}

void EventDispatch::StopDispatch()
{
    running_ = false;
}

void EventDispatch::StopDispatchInThread()
{
    running_ = false;
    if (eventThread_.joinable()) {
        eventThread_.join();
    }
}

#else

void EventDispatch::AddEvent(SOCKET fd, uint8_t socketEvent)
{
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLPRI | EPOLLERR | EPOLLHUP;
	ev.data.fd = fd;
	if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) != 0) {
		printf("epoll_ctl() failed, errno = %d", errno);
	}
}

void EventDispatch::RemoveEvent(SOCKET fd, uint8_t socketEvent)
{
	if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL) != 0) {
		printf("epoll_ctl failed, errno=%d", errno);
	}
}

void EventDispatch::StartDispatch(uint32_t waitTimeout)
{
	struct epoll_event events[1024];
	int nfds = 0;

    if (running_) {
        return;
    }

    running_ = true;
    
	while (running_) {

		nfds = epoll_wait(epollfd_, events, 1024, waitTimeout);
		for (int i = 0; i < nfds; ++i) {
			int ev_fd = events[i].data.fd;
            BaseSocketPtr socket = FindBaseSocket(ev_fd);
            if (!socket) {
                continue;
            }
            
            #ifdef EPOLLRDHUP
            if (events[i].events & EPOLLRDHUP) {
                socket->OnClose();
            }
            #endif

			if (events[i].events & EPOLLIN) {
				socket->OnRead();
			}

			if (events[i].events & EPOLLOUT) {
				socket->OnWrite();
			}

			if (events[i].events & (EPOLLPRI | EPOLLERR | EPOLLHUP)) {
				socket->OnClose();
			}
		}

		CheckTimer();
        CheckLoop();
	}
}

void EventDispatch::StopDispatch()
{
    running_ = false;
}

#endif  // _WIN32
