/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: UserInfo
 */

#include "stdafx.h"
#include "user_info.h"
#include "im_pdu_base.h"
#include "im.base.pb.h"

UserInfo::UserInfo()
{

}

UserInfo::~UserInfo()
{

}

void UserInfo::AddClientType(uint32_t clientType)
{
    auto it = clientTypeList_.find(clientType);
    if (it != clientTypeList_.end()) {
        it->second += 1;
    } else {
        clientTypeList_[clientType] = 1;
    }
}

void UserInfo::RemoveClientType(uint32_t clientType)
{
    auto it = clientTypeList_.find(clientType);
    if (it != clientTypeList_.end()) {
        uint32_t count = it->second;
        --count;
        if (count > 0) {
            it->second = count;
        } else {
            clientTypeList_.erase(clientType);
        }
    }
}

bool UserInfo::FindRouteConn(RouteConnPtr conn)
{
    auto it = routeConnSet_.find(conn);
    return (it != routeConnSet_.end());
}

uint32_t UserInfo::GetCountByClientType(uint32_t clientType)
{
    auto it = clientTypeList_.find(clientType);
    if (it != clientTypeList_.end()) {
        return it->second;
    } else {
        return 0;
    }
}

uint32_t UserInfo::GetStatus()
{
    uint32_t status = im::base::USER_STATUS_ONLINE;
    return status;
}