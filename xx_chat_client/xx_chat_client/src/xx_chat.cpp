/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "xx_chat.h"
#include "main_win.h"
#include "login\login_module.h"
#include "core\event_manager.h"


XXChat* XXChat::instance_ = NULL;

XXChat::XXChat()
{
    connect(z::core::GetEventManager(), SIGNAL(TriggerEvent(Event* /*event*/)),
        z::core::GetEventManager(), SLOT(OnEventTrigger(Event* /*event*/)));
}

XXChat::~XXChat()
{
    if (mainWin_) {
        mainWin_->close();
        delete mainWin_;
        mainWin_ = NULL;
    }
}

void XXChat::Start()
{
    z::login::GetLoginModule()->ShowLoginDialog();
    mainWin_ = new MainWin();
}
