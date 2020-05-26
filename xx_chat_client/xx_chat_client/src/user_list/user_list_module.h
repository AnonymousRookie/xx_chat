/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <map>
#include <string>
#include "defines.h"
#include "..\core\operation.h"
#include "..\core\module_base.h"

class ImPdu;

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(user_list)

class UserInfo
{
public:
    UserInfo() = default;
    ~UserInfo() = default;

private:
    std::string userid_;
    std::string username_;
};

typedef std::map<std::string, UserInfo> UserInfoMap;

class IUserListModule : public z::core::IPduPacketParse, public z::core::ModuleBase
{
public:
    virtual void GetAllUserInfo(UserInfoMap& userInfoMap) = 0;
};

IUserListModule* GetUserListModule();


class UserListModule final : public IUserListModule
{
public:
    UserListModule();
    virtual ~UserListModule();

    virtual void OnPacket(std::shared_ptr<ImPdu> pdu);
    virtual void GetAllUserInfo(UserInfoMap& userInfoMap);
    
private:
    UserInfoMap userInfoMap_;  // 所有用户信息

};

NAMESPACE_END(user_list)
NAMESPACE_END(z)