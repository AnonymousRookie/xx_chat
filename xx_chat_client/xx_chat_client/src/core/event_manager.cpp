/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <algorithm>
#include "base\src\defines.h"
#include "base\src\util\utils.h"
#include "event_manager.h"
#include "event.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

EventManager* GetEventManager()
{
    static EventManager eventManager;
    return& eventManager;
}

EventManager::EventManager()
{
    spTimer_ = std::make_shared<QTimer>(this);
    connect(spTimer_.get(), SIGNAL(timeout()), this, SLOT(OnTimeout()));
}

EventManager::~EventManager()
{
}

void EventManager::AsyncFireEvent(Event* event)
{
    emit TriggerEvent(event);
}

void EventManager::Startup()
{
    spTimer_->start(100);
}

void EventManager::Shutdown()
{
    spTimer_->stop();
}

void EventManager::OnEventTrigger(Event* event)
{
    event->Process();
}

void EventManager::OnTimeout()
{
    uint64_t currTick = z::utils::GetTickCount();
    for (auto event : lstTimerEvent_) {
        if (currTick >= event->nextTick_) {
            event->nextTick_ = currTick + event->interval_;
            event->Process();
        }
    }
}

void EventManager::RegisterTimerEvent(std::shared_ptr<TimerEvent> event)
{
    auto iter = std::find_if(lstTimerEvent_.begin(), lstTimerEvent_.end(), 
        [&](const std::shared_ptr<TimerEvent>& item) {
        return item->timerEventName_ == event->timerEventName_;
    });
    if (iter == lstTimerEvent_.end()) {
        lstTimerEvent_.push_back(event);
    }
}

void EventManager::UnRegisterTimerEvent(std::shared_ptr<TimerEvent> event)
{
    Z_CHECK(event);
    auto iter = std::find_if(lstTimerEvent_.begin(), lstTimerEvent_.end(), 
        [&](const std::shared_ptr<TimerEvent>& item) {
        return item->timerEventName_ == event->timerEventName_;
    });
    if (iter != lstTimerEvent_.end()) {
        iter = lstTimerEvent_.erase(iter);
    }
}


NAMESPACE_END(core)
NAMESPACE_END(z)

