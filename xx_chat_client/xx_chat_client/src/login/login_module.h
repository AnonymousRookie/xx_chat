/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include "defines.h"
#include "..\core\operation.h"
#include "..\core\module_base.h"

class ImPdu;
class MainWin;

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(login)

class LoginWin;

class ILoginModule : public z::core::IPduPacketParse, public z::core::ModuleBase
{
public:
    virtual bool ShowLoginDialog() = 0;
    virtual void NotifyLoginDone(std::shared_ptr<ImPdu> pdu) = 0;
};

ILoginModule* GetLoginModule();


class LoginModule final : public ILoginModule
{
public:
    LoginModule();
    virtual ~LoginModule();

    virtual void OnPacket(std::shared_ptr<ImPdu> pdu);

public:
    virtual bool ShowLoginDialog();
    virtual void NotifyLoginDone(std::shared_ptr<ImPdu> pdu);

private:
    std::shared_ptr<LoginWin> loginDlg_;
    std::shared_ptr<MainWin> mainWin_;
};

NAMESPACE_END(login)
NAMESPACE_END(z)