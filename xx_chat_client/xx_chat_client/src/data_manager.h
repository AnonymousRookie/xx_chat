﻿/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct UserInfo
{
    UserInfo() {}
    UserInfo(uint32_t id, const std::string& name) : userId(id), userName(name) {}
    uint32_t userId;
    std::string userName;
};

class DataManager
{
public:
    DataManager();
    ~DataManager();

    static std::shared_ptr<DataManager> GetInstance();

    void SetCurLoginUserId(uint32_t id);
    uint32_t GetCurLoginUserId();

    void AddUserInfo(UserInfo userInfo);
    bool GetUserInfo(const std::string& userName, UserInfo& userInfo);

    void SetLoginUserInfo(const std::string& name, const std::string& passwd);
    void GetLoginUserInfo(std::string& name, std::string& passwd);

    void SetMsgServerInfo(const std::string& ip, const uint16_t& port);
    void GetMsgServerInfo(std::string& ip, uint16_t& port);

private:
    static std::shared_ptr<DataManager> instance_;
    uint32_t curLoginUserId_ = 0;
    std::vector<UserInfo> userInfos_;

    std::string loginName_;
    std::string loginPasswd_;

    std::string msgServerIp_;
    uint16_t msgServerPort_;
};

