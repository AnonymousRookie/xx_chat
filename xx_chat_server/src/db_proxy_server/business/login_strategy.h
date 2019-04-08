/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 登录验证策略
 */

#ifndef DB_PROXY_SERVER_BUSINESS_LOGIN_STRATEGY_H
#define DB_PROXY_SERVER_BUSINESS_LOGIN_STRATEGY_H

#include <string>
#include "im.base.pb.h"

class LoginStrategy
{
public:
    virtual bool DoLogin(const std::string& name, 
                         const std::string& password,
                         im::base::UserInfo& user) = 0;
};


#endif  // DB_PROXY_SERVER_BUSINESS_LOGIN_STRATEGY_H