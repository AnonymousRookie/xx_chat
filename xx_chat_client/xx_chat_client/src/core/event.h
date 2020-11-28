/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <functional>
#include <memory>
#include "base\src\defines.h"
#include "base\src\network\util_pdu.h"

class ImPdu;

enum EventId {
    EventId_Invalid = 0,
    EventId_LoginDone,
    EventId_FriendList,
    EventId_MsgData,
    EventId_TcpClientState,
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

    void SetData(void* data, uint32_t len);

private:
    EventId eventId_;
    std::shared_ptr<ImPdu> pdu_;
    SimpleBuffer buf_;

public:
    std::shared_ptr<ModuleSubject> pModuleSubject_;
};


class TimerEvent final : public IEvent
{
public:
    TimerEvent();
    virtual ~TimerEvent();

    void Process();

public:
    std::string timerEventName_;
    std::function<void()> callback_;
    uint64_t interval_ = 0;
    uint64_t nextTick_ = 0;
};


NAMESPACE_END(core)
NAMESPACE_END(z)
