/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <cstdint>
#include <unordered_map>
#include "defines.h"
#include "..\core\operation.h"
#include "..\core\module_base.h"

class MessageEntity;

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(session)


class ISessionModule : public z::core::IPduPacketParse, public z::core::ModuleBase
{
public:
    virtual void SendMsg(std::shared_ptr<MessageEntity> msg) = 0;
};

ISessionModule* GetSessionModule();

class SessionModule final : public ISessionModule
{
public:
    SessionModule();
    virtual ~SessionModule();

    virtual void OnPacket(std::shared_ptr<ImPdu> pdu);


    virtual void SendMsg(std::shared_ptr<MessageEntity> msg);

};


NAMESPACE_END(session)
NAMESPACE_END(z)