/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 处理用户登录请求
 */

#ifndef DB_PROXY_SERVER_BUSINESS_LOGIN_H
#define DB_PROXY_SERVER_BUSINESS_LOGIN_H

#include "im_pdu_base.h"
#include "defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(business)

void DoLogin(std::shared_ptr<ImPdu> pdu, uint32_t connUuid);

NAMESPACE_END(business)
NAMESPACE_END(z)


#endif  // DB_PROXY_SERVER_BUSINESS_LOGIN_H