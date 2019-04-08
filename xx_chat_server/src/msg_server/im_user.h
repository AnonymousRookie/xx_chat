/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ImUser
 */

#ifndef MSG_SERVER_IM_USER_H
#define MSG_SERVER_IM_USER_H

#include <string>
#include <map>
#include <list>
#include <set>
#include "defines.h"

class MsgConn;

class ImUser
{
public:
    ImUser(const std::string& userName);
    ~ImUser();

    void SetUserId(uint32_t userId) { userId_ = userId; }
    uint32_t GetUserId() { return userId_; }

    bool IsValidate() { return validate_; }
    void SetValidate(bool validate = true) { validate_ = validate; }

    user_conn_t GetUserConn();

    bool IsMsgConnEmpty() { return connMap_.empty(); }

    std::map<uint32_t, MsgConn*>& GetMsgConnMap() { return connMap_; }

    void AddUnValidateMsgConn(MsgConn* conn) { unValidateConnSet_.insert(conn); };
    void DelUnValidateMsgConn(MsgConn* conn) { unValidateConnSet_.erase(conn); }
    MsgConn* GetUnValidateMsgConn(uint32_t handle);

private:
    uint32_t userId_;
    std::string userName_;
    uint32_t loginStatus_;  // 登录状态(1: on, 0: off)
    bool validate_;
    std::map<uint32_t/*handle*/, MsgConn*> connMap_;
    std::set<MsgConn*> unValidateConnSet_;
};

typedef std::map<uint32_t/*userId*/, ImUser*> ImUserIdMap;
typedef std::map<std::string/*userName*/, ImUser*> ImUserNameMap;

class ImUserManager
{
public:
    ImUserManager();
    ~ImUserManager();

    static ImUserManager* GetInstance();
    ImUser* GetImUserById(uint32_t userId);
    ImUser* GetImUserByName(const std::string& userName);

    bool AddImUserByName(const std::string& userName, ImUser* user);
    void RemoveImUserByName(const std::string& userName);

    bool AddImUserById(uint32_t userId, ImUser* user);
    void RemoveImUserById(uint32_t userId);

    void GetOnlineUserInfo(std::list<user_stat_t>* online_user_info);
    void GetUserConnCnt(std::list<user_conn_t>* user_conn_list, uint32_t* total_conn_cnt);

private:
    ImUserIdMap userIdMap_;
    ImUserNameMap userNameMap_;
};


#endif  // MSG_SERVER_IM_USER_H