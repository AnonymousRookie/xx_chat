/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <memory>
#include "base\src\defines.h"
#include "base\src\network\util_pdu.h"

class ImPdu;

enum EventId {
    EventId_Invalid = 0,
    EventId_LoginDone,
    EventId_FriendList,
    EventId_MsgData,
};

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

class ModuleSubject;

struct IEvent
{
public:
    virtual void Process() = 0;
};

class Event final : public IEvent
{
public:
    Event(std::shared_ptr<ModuleSubject> pModuleSubject);
    virtual ~Event();

public:
    void Process();
    void SetEventId(EventId eventId) {
        eventId_ = eventId;
    }
    void SetEventData(std::shared_ptr<ImPdu> pdu) {
        pdu_ = pdu;
    }
private:
    EventId eventId_;
    std::shared_ptr<ImPdu> pdu_;

public:
    std::shared_ptr<ModuleSubject> pModuleSubject_;
};

NAMESPACE_END(core)
NAMESPACE_END(z)
