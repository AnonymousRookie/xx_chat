/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "login_module.h"
#include "login_win.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(login)

ILoginModule* GetLoginModule()
{
    static LoginModule module;
    return &module;
}

LoginModule::LoginModule()
{
}

LoginModule::~LoginModule()
{
}

void LoginModule::OnPacket()
{
}

bool LoginModule::ShowLoginDialog()
{
    LoginWin* dlg = new LoginWin();
    dlg->show();
    return true;
}

void LoginModule::NotifyLoginDone()
{

}

NAMESPACE_END(login)
NAMESPACE_END(z)