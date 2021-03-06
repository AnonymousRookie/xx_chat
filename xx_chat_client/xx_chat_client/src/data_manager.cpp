﻿/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "data_manager.h"

std::shared_ptr<DataManager> DataManager::instance_ = nullptr;

DataManager::DataManager()
{

}

DataManager::~DataManager()
{

}

std::shared_ptr<DataManager> DataManager::GetInstance()
{
    if (!instance_) {
        instance_ = std::make_shared<DataManager>();
    }
    return instance_;
}

void DataManager::SetCurLoginUserId(uint32_t id)
{
    curLoginUserId_ = id;
}

uint32_t DataManager::GetCurLoginUserId()
{
    return curLoginUserId_;
}

void DataManager::AddUserInfo(UserInfo userInfo)
{
    userInfos_.push_back(userInfo);
}

bool DataManager::GetUserInfo(const std::string& userName, UserInfo& userInfo)
{
    auto findIter = std::find_if(userInfos_.begin(), userInfos_.end(), 
        [&] (const UserInfo& userInfo)
    {
        return userName == userInfo.userName;
    });

    if (findIter != userInfos_.end()) {
        userInfo = *findIter;
        return true;
    }
    return false;
}

void DataManager::SetLoginUserInfo(const std::string& name, const std::string& passwd)
{
    loginName_ = name;
    loginPasswd_ = passwd;
}

void DataManager::GetLoginUserInfo(std::string& name, std::string& passwd)
{
    name = loginName_;
    passwd = loginPasswd_;
}

void DataManager::SetMsgServerInfo(const std::string& ip, const uint16_t& port)
{
    msgServerIp_ = ip;
    msgServerPort_ = port;
}

void DataManager::GetMsgServerInfo(std::string& ip, uint16_t& port)
{
    ip = msgServerIp_;
    port = msgServerPort_;
}
