/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include "base\src\defines.h"
#include "event.h"
#include <string>
#include <memory>
#include <functional>


#define BIND_CALLBACK_1(func) std::bind(&func, this, std::placeholders::_1)
#define BIND_CALLBACK_2(func) std::bind(&func, this, std::placeholders::_1, std::placeholders::_2)

typedef std::function<void(std::string)> Delegate;

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)


class ModuleObserver
{
public:
    virtual void OnNotify(EventId eventId, std::shared_ptr<ImPdu> pdu) = 0;
    virtual void OnNotify(EventId eventId, void* data, uint32_t len) = 0;
};

NAMESPACE_END(core)
NAMESPACE_END(z)
