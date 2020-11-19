/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <memory>
#include <vector>
#include <QObject>

class FriendChatDlg;

class ChatDlgManager : public QObject
{
    Q_OBJECT

public:
    static ChatDlgManager* Instance()
    {
        if (!instance_) {
            instance_ = new ChatDlgManager();
        }
        return instance_;
    }

    ~ChatDlgManager();

public:
    void OpenFriendChatDlg(const QString& name);

private:
    ChatDlgManager();
    ChatDlgManager(const ChatDlgManager&) = delete;
    ChatDlgManager& operator=(const ChatDlgManager&) = delete;

private:
    std::vector<std::shared_ptr<FriendChatDlg>> openedFriendChatDlgVector_;

private:
    static ChatDlgManager* instance_;
};