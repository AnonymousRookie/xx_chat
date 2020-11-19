/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "chat_dlg_manager.h"
#include "friend_chat_dlg.h"

ChatDlgManager* ChatDlgManager::instance_ = nullptr;

ChatDlgManager::ChatDlgManager()
{
}

ChatDlgManager::~ChatDlgManager()
{
}

void ChatDlgManager::OpenFriendChatDlg(const QString& name)
{
    std::shared_ptr<FriendChatDlg> dlg = std::make_shared<FriendChatDlg>();
    dlg->SetFriendName(name);
    openedFriendChatDlgVector_.push_back(dlg);
}