/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 登录验证
 */

#ifndef DB_PROXY_SERVER_BUSINESS_INTER_LOGIN_H
#define DB_PROXY_SERVER_BUSINESS_INTER_LOGIN_H

#include <string>
#include "im.base.pb.h"
#include "login_strategy.h"

// 内部数据库验证策略
class InterLoginStrategy : public LoginStrategy
{
public:
    virtual bool DoLogin(const std::string& name,
                         const std::string& password,
                         im::base::UserInfo& user);
};


#endif  // DB_PROXY_SERVER_BUSINESS_INTER_LOGIN_H