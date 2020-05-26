/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#ifndef DB_PROXY_SERVER_BUSINESS_USER_MODEL_H
#define DB_PROXY_SERVER_BUSINESS_USER_MODEL_H

#include <list>
#include "im_pdu_base.h"
#include "im.base.pb.h"
#include "defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(business)

class UserModel
{
public:
    static UserModel* GetInstance();
    ~UserModel();

    void GetUserInfos(std::list<im::base::UserInfo>& userInfoList);

private:
    UserModel();

private:
    static UserModel* instance_;
};

NAMESPACE_END(business)
NAMESPACE_END(z)


#endif  // DB_PROXY_SERVER_BUSINESS_USER_MODEL_H