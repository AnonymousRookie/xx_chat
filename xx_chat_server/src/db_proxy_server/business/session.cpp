/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 处理会话
 */

#include "session.h"
#include "../proxy_conn.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(business)

void ProcessSessionMsg(std::shared_ptr<ImPdu> pdu, uint32_t connUuid)
{
    ProxyConn::AddResponsePdu(connUuid, pdu);
}

NAMESPACE_END(business)
NAMESPACE_END(z)