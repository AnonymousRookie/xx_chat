/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "session_module.h"
#include "..\message\message_entity.h"
#include "..\core\operation_manager.h"
#include "..\core\tcp_client_module.h"
#include "..\protocol\im.message.pb.h"
#include "..\data_manager.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(session)

ISessionModule* GetSessionModule()
{
    static SessionModule module;
    return &module;
}

SessionModule::SessionModule()
{

}

SessionModule::~SessionModule()
{

}

void SessionModule::OnPacket(std::shared_ptr<ImPdu> pdu)
{
    z::session::GetSessionModule()->AsynNotifyObserver(EventId::EventId_MsgData, pdu);
}

void SessionModule::SendMsg(std::shared_ptr<MessageEntity> msg)
{
    z::core::GetOperationManager()->StartOperationWithLambda(
        [=] 
    {
        im::message::MsgData msgData;
        msgData.set_msg_type(im::base::MsgType::MSG_TYPE_SINGLE_TEXT);
        msgData.set_from(msg->from_);
        msgData.set_msg_id(msg->msgId_);
        msgData.set_to(msg->to_);
        msgData.set_create_time(msg->msgTime_);
        msgData.set_msg_data(msg->msgContent_);

        z::core::GetTcpClientModule()->SendPacket(im::base::ServiceID::SID_MSG,
            im::base::MessageCmdID::CID_MSG_DATA, &msgData);
    });
}

NAMESPACE_END(session)
NAMESPACE_END(z)

