/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: Á¬½Ómsg_server
 */

#pragma once

#include "defines.h"
#include "..\core\operation.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(login)

enum
{
    LOGIN_MSG_SERVER_SUCCESS = 0,
    LOGIN_MSG_SERVER_FAILED,
};

struct LoginMsgServerParam
{
    uint8_t result = LOGIN_MSG_SERVER_FAILED;
    uint32_t serverTime = 0;
    std::string msgServerIp;
    uint16_t msgServerPort = 0;
    std::string username;
    std::string password;
    std::string err;
};

typedef std::function<void(std::shared_ptr<void>)> LoginDoneCallback;

class LoginOperation : public z::core::CallbackOperation
{
public:
    LoginOperation(z::core::OperationDelegate callback, LoginMsgServerParam& param);
    virtual ~LoginOperation();

    virtual void ProcessOperation();

private:
    void OnLoginDone(std::shared_ptr<void> param);

private:
    LoginMsgServerParam loginParam_;
};

NAMESPACE_END(login)
NAMESPACE_END(z)