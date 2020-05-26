/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "network\im_pdu_base.h"
#include "..\core\operation_manager.h"
#include "..\core\tcp_client_module.h"
#include "..\protocol\im.buddy.pb.h"
#include "..\protocol\im.base.pb.h"
#include "user_list_module.h"


NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(user_list)

IUserListModule* GetUserListModule()
{
    static UserListModule module;
    return &module;
}

UserListModule::UserListModule()
{
}

UserListModule::~UserListModule()
{
}

void UserListModule::OnPacket(std::shared_ptr<ImPdu> pdu)
{
    switch (pdu->GetCommandId()) {
    case im::base::BuddyListCmdID::CID_BUDDY_LIST_ALL_USER_RSP:
        z::user_list::GetUserListModule()->AsynNotifyObserver(EventId::EventId_FriendList, pdu);
        break;
    default:
        break;
    }
}

void UserListModule::GetAllUserInfo(UserInfoMap& userInfoMap)
{

}

NAMESPACE_END(user_list)
NAMESPACE_END(z)