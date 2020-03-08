/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: Á¬½Ólogin_server
 */

#pragma once

#include <QWidget>
#include "defines.h"
#include "ui_login_win.h"
#include "../core/http_operation.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(login)

enum 
{
    LOGIN_LOGIN_SERVER_SUCCESS = 0,
    LOGIN_LOGIN_SERVER_FAILED,
};

struct DoLoginServerParam
{
    uint8_t result = LOGIN_LOGIN_SERVER_FAILED;
    std::string msgServerIp;
    uint16_t msgServerPort = 0;
    std::string resMsg;
};

class LoginHttpServerOperation : public z::core::IHttpOperation
{
public:
    LoginHttpServerOperation(z::core::OperationDelegate callback, DoLoginServerParam& param);
    virtual ~LoginHttpServerOperation() = default;

public:
    virtual void ProcessOperation();

private:
    DoLoginServerParam loginServerParam_;
};

NAMESPACE_END(login)
NAMESPACE_END(z)