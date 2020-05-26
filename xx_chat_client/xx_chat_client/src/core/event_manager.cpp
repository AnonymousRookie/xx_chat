/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include "base\src\defines.h"
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

}

EventManager::~EventManager()
{

}

void EventManager::AsyncFireEvent(Event* event)
{
    emit TriggerEvent(event);
}

void EventManager::OnEventTrigger(Event* event)
{
    event->Process();
}


NAMESPACE_END(core)
NAMESPACE_END(z)
