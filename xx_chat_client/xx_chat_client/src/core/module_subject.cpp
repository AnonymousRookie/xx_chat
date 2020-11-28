/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "module_subject.h"
#include "event_manager.h"
#include "event.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)


ModuleSubject::ModuleSubject()
{
}

ModuleSubject::~ModuleSubject()
{
}

void ModuleSubject::AddObserver(ModuleObserver* observer)
{
    std::lock_guard<std::mutex> lock(mutex_);
    moduleObserverVector_.push_back(observer);
}

void ModuleSubject::RemoveObserver(ModuleObserver* observer)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = std::remove_if(moduleObserverVector_.begin(), moduleObserverVector_.end(),
        [=](ModuleObserver* item) {
        return item == observer;
    });
    if (iter != moduleObserverVector_.end()) {
        moduleObserverVector_.erase(iter, moduleObserverVector_.end());
    }
}

void ModuleSubject::AsynNotifyObserver(EventId eventId, std::shared_ptr<ImPdu> pdu)
{
    Event* event = new Event(shared_from_this());
    event->SetEventId(eventId);
    event->SetEventData(pdu);
    z::core::GetEventManager()->AsyncFireEvent(event);
}

void ModuleSubject::AsynNotifyObserver(EventId eventId, void* data, int len)
{
    Event* event = new Event(shared_from_this());
    event->SetEventId(eventId);
    event->SetData(data, len);
    z::core::GetEventManager()->AsyncFireEvent(event);
}

std::vector<ModuleObserver*> ModuleSubject::GetModuleObservers()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return moduleObserverVector_;
}

NAMESPACE_END(core)
NAMESPACE_END(z)
