/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: HandlerMapManager
 */

#include "stdafx.h"
#include "im.base.pb.h"
#include "business/login.h"
#include "business/user_action.h"
#include "handler_map.h"

HandlerMapManager* HandlerMapManager::handlerMapManagerInstance_ = NULL;

HandlerMapManager::HandlerMapManager()
{
}

HandlerMapManager::~HandlerMapManager()
{
}

HandlerMapManager* HandlerMapManager::GetInstance()
{
    if (!handlerMapManagerInstance_) {
        handlerMapManagerInstance_ = new HandlerMapManager();
        handlerMapManagerInstance_->Init();
    }
    return handlerMapManagerInstance_;
}

void HandlerMapManager::Init()
{
    // login validate
    handlerMap_.insert(std::make_pair(im::base::OtherCmdID::CID_OTHER_LOGIN_VALIDATE_REQ, z::business::DoLogin));
    handlerMap_.insert(std::make_pair(im::base::BuddyListCmdID::CID_BUDDY_LIST_ALL_USER_REQ, z::business::GetAllUser));
}

pdu_handler_t HandlerMapManager::GetHandler(uint32_t pduType)
{
    auto it = handlerMap_.find(pduType);
    if (it != handlerMap_.end()) {
        return it->second;
    }
    return NULL;
}
