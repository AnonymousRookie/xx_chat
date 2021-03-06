﻿/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "base\src\defines.h"
#include "event.h"
#include "module_subject.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

Event::Event(std::shared_ptr<ModuleSubject> pModuleSubject)
    : pModuleSubject_(pModuleSubject)
{

}

Event::~Event()
{

}

void Event::Process()
{
    auto& moduleObservers = pModuleSubject_->GetModuleObservers();
    auto iter = moduleObservers.begin();
    for (; iter != moduleObservers.end(); ++iter) {
        ModuleObserver* moduleObserver = *iter;
        moduleObserver->OnNotify(eventId_, pdu_);
        moduleObserver->OnNotify(eventId_, (void*)buf_.GetBuffer(), buf_.GetWriteOffset());
    }
}

void Event::SetData(void* data, uint32_t len)
{
    buf_.Write(data, len);
}

TimerEvent::TimerEvent()
{

}

TimerEvent::~TimerEvent()
{

}

void TimerEvent::Process()
{
    callback_();
}

NAMESPACE_END(core)
NAMESPACE_END(z)
