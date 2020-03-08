/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include "defines.h"
#include "..\core\operation.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(login)

class ILoginModule : public z::core::IPduPacketParse
{
public:
    virtual bool ShowLoginDialog() = 0;
    virtual void NotifyLoginDone() = 0;
};

ILoginModule* GetLoginModule();


class LoginModule final : public ILoginModule
{
public:
    LoginModule();
    virtual ~LoginModule();

    virtual void OnPacket();

public:
    virtual bool ShowLoginDialog();
    virtual void NotifyLoginDone();
};

NAMESPACE_END(login)
NAMESPACE_END(z)