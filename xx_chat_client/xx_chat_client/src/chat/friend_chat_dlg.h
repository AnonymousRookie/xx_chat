/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <memory>
#include <QString>
#include "ui_friend_chat_dlg.h"
#include "core\module_observer.h"
#include "core\event.h"

class FriendChatDlg : public QWidget, public z::core::ModuleObserver
{
    Q_OBJECT

public:
    FriendChatDlg(QWidget *parent = Q_NULLPTR);
    ~FriendChatDlg();

public:
    void SetFriendName(const QString& name);
    const QString& GetFriendName();

private slots:
    void OnSendMsg();

private:
    void OnNotify(EventId eventId, std::shared_ptr<ImPdu> pdu);
    void OnMsgDataComming(std::shared_ptr<ImPdu> pdu);

private:
    QString friendName_;

private:
    Ui::FriendChatDlg ui;
};