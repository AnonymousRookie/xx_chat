/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 处理会话
 */

#ifndef DB_PROXY_SERVER_BUSINESS_SESSION_H
#define DB_PROXY_SERVER_BUSINESS_SESSION_H

#include "im_pdu_base.h"
#include "defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(business)

void ProcessSessionMsg(std::shared_ptr<ImPdu> pdu, uint32_t connUuid);

NAMESPACE_END(business)
NAMESPACE_END(z)


#endif  // DB_PROXY_SERVER_BUSINESS_SESSION_H