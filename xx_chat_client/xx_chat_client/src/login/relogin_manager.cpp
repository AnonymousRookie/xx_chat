/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 断线重连
 */

#include <functional>
#include "relogin_manager.h"
#include "..\protocol\im.login.pb.h"
#include "..\core\tcp_client_module.h"
#include "..\core\operation_manager.h"
#include "..\core\event.h"
#include "..\core\event_manager.h"
#include "login_operation.h"
#include "data_manager.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(login)

std::shared_ptr<ReLoginManager> ReLoginManager::instance_ = nullptr;

ReLoginManager::ReLoginManager()
{

}


ReLoginManager::~ReLoginManager()
{

}

std::shared_ptr<ReLoginManager> ReLoginManager::GetInstance()
{
    if (!instance_) {
        instance_ = std::make_shared<ReLoginManager>();
    }
    return instance_;
}

void ReLoginManager::DoReLogin()
{
    if (!timerEvent_) {
        timerEvent_ = std::make_shared<z::core::TimerEvent>();
        timerEvent_->timerEventName_ = "ReLoginManager_DoReLogin_Timer";
        timerEvent_->interval_ = 3000;
    }

    timerEvent_->callback_ = [&]() {
        ReLogin();
    };

    z::core::GetEventManager()->RegisterTimerEvent(timerEvent_);
}

void ReLoginManager::ReLogin()
{
    // 开始连接msg_server
    std::string username;
    std::string passwd;
    std::string msgServerIp;
    uint16_t msgServerPort;

    DataManager::GetInstance()->GetLoginUserInfo(username, passwd);
    DataManager::GetInstance()->GetMsgServerInfo(msgServerIp, msgServerPort);

    LoginMsgServerParam loginParam;
    loginParam.username = username;
    loginParam.password = passwd;

    loginParam.msgServerIp = msgServerIp;
    loginParam.msgServerPort = msgServerPort;

    auto pOperation = std::make_shared<LoginOperation>(
        std::bind(&ReLoginManager::OnOperationCallback, this, std::placeholders::_1), loginParam);
    z::core::GetOperationManager()->StartOperation(pOperation, 0);
}


void ReLoginManager::OnOperationCallback(std::shared_ptr<void> param)
{
    z::core::GetEventManager()->UnRegisterTimerEvent(timerEvent_);
}

NAMESPACE_END(login)
NAMESPACE_END(z)

