/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ImUser
 */

#include "stdafx.h"
#include "im.base.pb.h"
#include "msg_conn.h"
#include "im_user.h"


ImUser::ImUser(const std::string& userName)
    : userName_(userName)
    , userId_(0)
    , loginStatus_(im::base::UserStatType::USER_STATUS_OFFLINE)
{

}

ImUser::~ImUser()
{

}

user_conn_t ImUser::GetUserConn()
{
    uint32_t connCnt = 0;
    for (auto it = connMap_.begin(); it != connMap_.end(); ++it) {
        MsgConn* conn = it->second;
        if (conn->IsOpen()) {
            ++connCnt;
        }
    }
    user_conn_t user_conn = {userId_, connCnt};
    return user_conn;
}

MsgConn* ImUser::GetUnValidateMsgConn(uint32_t handle)
{
    for (auto it = unValidateConnSet_.begin(); it != unValidateConnSet_.end(); ++it) {
        MsgConn* conn = *it;
        if (conn->GetHandle() == handle) {
            return conn;
        }
    }
    return nullptr;
}

MsgConn* ImUser::GetMsgConn(uint32_t handle)
{
    MsgConn* conn = nullptr;
    auto iter = connMap_.find(handle);
    if (iter != connMap_.end()) {
        conn = iter->second;
    }
    return conn;
}

ImUserManager::ImUserManager()
{

}

ImUserManager::~ImUserManager()
{

}

ImUserManager* ImUserManager::GetInstance()
{
    static ImUserManager imUserManager;
    return &imUserManager;
}

ImUser* ImUserManager::GetImUserById(uint32_t userId)
{
    auto it = userIdMap_.find(userId);
    if (it != userIdMap_.end()) {
        return it->second;
    }
    return nullptr;
}

ImUser* ImUserManager::GetImUserByName(const std::string& userName)
{
    auto it = userNameMap_.find(userName);
    if (it != userNameMap_.end()) {
        return it->second;
    }
    return nullptr;
}

bool ImUserManager::AddImUserByName(const std::string& userName, ImUser* user)
{
    if (GetImUserByName(userName) == NULL) {
        userNameMap_.insert(std::make_pair(userName, user));
        return true;
    }
    return false;
}

void ImUserManager::RemoveImUserByName(const std::string & userName)
{
    userNameMap_.erase(userName);
}

bool ImUserManager::AddImUserById(uint32_t userId, ImUser* user)
{
    if (GetImUserById(userId) == NULL) {
        userIdMap_.insert(std::make_pair(userId, user));
        return true;
    }
    return false;
}

void ImUserManager::RemoveImUserById(uint32_t userId)
{
    userIdMap_.erase(userId);
}

void ImUserManager::GetOnlineUserInfo(std::list<user_stat_t>* online_user_info)
{
    user_stat_t status;
    ImUser* user = NULL;

    for (auto it = userIdMap_.begin(); it != userIdMap_.end(); ++it) {
        user = it->second;
        if (user->IsValidate() == false) {
            continue;
        }
        auto& connMap = user->GetMsgConnMap();
        for (auto it = connMap.begin(); it != connMap.end(); ++it) {
            MsgConn* conn = it->second;
            if (conn->IsOpen()) {
                status.user_id = user->GetUserId();
                status.status = conn->GetOnlineStatus();
                online_user_info->push_back(status);
            }
        }
    }
}

void ImUserManager::GetUserConnCnt(std::list<user_conn_t>* user_conn_list, uint32_t* total_conn_cnt)
{
    *total_conn_cnt = 0;
    ImUser* user = NULL;
    for (auto it = userIdMap_.begin(); it != userIdMap_.end(); ++it) {
        user = it->second;
        if (user->IsValidate()) {
            auto userConn = user->GetUserConn();
            user_conn_list->push_back(userConn);
            *total_conn_cnt += userConn.conn_cnt;
        }
    }
}

MsgConn* ImUserManager::GetMsgConnByHandle(uint32_t userId, uint32_t handle)
{
    MsgConn* conn = NULL;
    ImUser* user = GetImUserById(userId);
    if (user) {
        conn = user->GetMsgConn(handle);
    }
    return conn;
}