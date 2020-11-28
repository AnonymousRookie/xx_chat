/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "friend_chat_dlg.h"
#include "session_module.h"
#include "..\message\message_entity.h"
#include "base\src\util\string_util.h"
#include "..\data_manager.h"
#include "base\src\defines.h"
#include "..\data_manager.h"
#include "..\protocol\im.message.pb.h"
#include "network\im_pdu_base.h"

FriendChatDlg::FriendChatDlg(QWidget *parent/* = Q_NULLPTR*/)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->show();

    connect(ui.btn_send_msg, SIGNAL(clicked()), this, SLOT(OnSendMsg()));
    z::session::GetSessionModule()->AddObserver(this);
}

FriendChatDlg::~FriendChatDlg()
{

}

void FriendChatDlg::SetFriendName(const QString& name) 
{ 
    friendName_ = name; 
    ui.label_friend_name->setText(name);
}

const QString& FriendChatDlg::GetFriendName() 
{ 
    return friendName_; 
}

void FriendChatDlg::OnSendMsg()
{
    QString& text = ui.textEdit_msg_send->toPlainText();

    UserInfo friendInfo;
    bool ret = DataManager::GetInstance()->GetUserInfo(z::utils::qStr2Str(friendName_), friendInfo);
    Z_CHECK(ret);

    std::shared_ptr<MessageEntity> msgEntity = std::make_shared<MessageEntity>();

    msgEntity->msgType_ = MSG_TYPE_TEXT_P2P;
    msgEntity->from_ = DataManager::GetInstance()->GetCurLoginUserId();
    msgEntity->to_ = friendInfo.userId;
    msgEntity->msgContent_ = z::utils::qStr2Str(text);

    z::session::GetSessionModule()->SendMsg(msgEntity);
}

void FriendChatDlg::OnNotify(EventId eventId, std::shared_ptr<ImPdu> pdu)
{
    switch (eventId)
    {
    case EventId_MsgData:
        OnMsgDataComming(pdu);
        break;
    default:
        break;
    }
}

void FriendChatDlg::OnNotify(EventId eventId, void* data, uint32_t len)
{

}

void FriendChatDlg::OnMsgDataComming(std::shared_ptr<ImPdu> pdu)
{
    im::message::MsgData msg;
    bool ret = msg.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength());

    ui.textEdit_msg_recv->setText(z::utils::str2qstr(msg.msg_data()));
}
