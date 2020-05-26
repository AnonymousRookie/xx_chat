/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ����msg_server
 */

#include <functional>
#include "login_operation.h"
#include "..\protocol\im.login.pb.h"
#include "..\core\tcp_client_module.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(login)

LoginOperation::LoginOperation(z::core::OperationDelegate callback, LoginMsgServerParam& param)
    : CallbackOperation(callback)
    , loginParam_(param)
{
}

LoginOperation::~LoginOperation()
{
}

void LoginOperation::ProcessOperation()
{
    // ����msg_server
    std::string ip = "";
    uint16_t port = 0;
    z::core::getTcpClientModule()->DoLogin(loginParam_.msgServerIp,
        loginParam_.msgServerPort,
        loginParam_.username, 
        loginParam_.password,
        std::bind(&LoginOperation::OnLoginDone, this, std::placeholders::_1));
}

void LoginOperation::OnLoginDone(std::shared_ptr<void> param)
{
    auto p = std::make_shared<LoginMsgServerParam>();
    p->result = LOGIN_MSG_SERVER_SUCCESS;
    SyncCallback(p);
}

NAMESPACE_END(login)
NAMESPACE_END(z)