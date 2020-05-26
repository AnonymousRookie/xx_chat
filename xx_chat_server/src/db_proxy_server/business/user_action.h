/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#ifndef DB_PROXY_SERVER_BUSINESS_USER_ACTION_H
#define DB_PROXY_SERVER_BUSINESS_USER_ACTION_H

#include "im_pdu_base.h"
#include "defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(business)

void GetAllUser(std::shared_ptr<ImPdu> pdu, uint32_t connUuid);

NAMESPACE_END(business)
NAMESPACE_END(z)


#endif  // DB_PROXY_SERVER_BUSINESS_USER_ACTION_H