/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include <functional>
#include <memory>
#include "util\logger.h"
#include "util\utils.h"
#include "..\protocol\im.login.pb.h"
#include "..\protocol\im.other.pb.h"
#include "..\login\login_module.h"
#include "..\login\login_operation.h"
#include "tcp_client_module.h"
#include "operation.h"

z::core::IPduPacketParse* getModule(uint16_t moduleId)
{
    z::core::IPduPacketParse* ret = NULL;
    switch (moduleId)
    {
    case im::base::SID_LOGIN:
        ret = z::login::GetLoginModule();
        break;
    default:
        break;
    }
    return ret;
}


NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

static ConnMap g_client_conn;

std::shared_ptr<ITcpClientModule> getTcpClientModule()
{
    static std::shared_ptr<TcpClientModule> module = std::make_shared<TcpClientModule>();
    return module;
}

TcpClientModule::TcpClientModule()
{
}

TcpClientModule::~TcpClientModule()
{

}

net_handle_t TcpClientModule::DoLogin(const std::string& ip, 
    uint16_t port, 
    const std::string & username, 
    const std::string & password,
    std::function<void(std::shared_ptr<void>)> loginDoneCallback)
{
    loginDoneCallback_ = loginDoneCallback;

    username_ = username;
    password_ = password;

    handle_ = z::net::netlib_connect(ip.c_str(), port, 
        std::bind(&ImConnCallback, std::placeholders::_1, 
            std::placeholders::_2, (void*)&g_client_conn)
    );

    if (handle_ != NETLIB_INVALID_HANDLE) {
        g_client_conn.insert(std::make_pair(handle_, shared_from_this()));
    }

    return handle_;
}

void TcpClientModule::OnConfirm()
{
    im::login::LoginReq msg;
    msg.set_username(username_);
    msg.set_password(password_);
    msg.set_online_status(im::base::UserStatType::USER_STATUS_ONLINE);

    ImPdu pdu;
    pdu.SetPBMsg(&msg);
    pdu.SetServiceId(im::base::ServiceID::SID_LOGIN);
    pdu.SetCommandId(im::base::LoginCmdID::CID_LOGIN_REQ_USERLOGIN);
    uint32_t seqNo = 0;
    pdu.SetSeqNum(seqNo);
    SendPdu(&pdu);
}

void TcpClientModule::OnClose()
{
    Close();
}

void TcpClientModule::Close()
{
    if (handle_ != NETLIB_INVALID_HANDLE) {
        z::net::netlib_close(handle_);
        g_client_conn.erase(handle_);
    }
}

void TcpClientModule::OnTimer(uint64_t currentTick)
{
    if (currentTick > lastSendTick_ + CLIENT_HEARTBEAT_INTERVAL) {
        im::other::HeartBeat msg;
        ImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(im::base::ServiceID::SID_OTHER);
        pdu.SetCommandId(im::base::OtherCmdID::CID_OTHER_HEARTBEAT);
        uint32_t seqNo = 0;
        pdu.SetSeqNum(seqNo);
        SendPdu(&pdu);
    }
    if (currentTick > lastRecvTick_ + CLIENT_TIMEOUT) {
        LOG_WARN("connect to msg_server timeout!");
        Close();
    }
}

void TcpClientModule::HandlePdu(ImPdu * pdu)
{
    switch (pdu->GetCommandId()) {
    case im::base::OtherCmdID::CID_OTHER_HEARTBEAT:
        break;
    case im::base::LoginCmdID::CID_LOGIN_RES_USERLOGIN:
        HandleLoginResponse(pdu);
        break;
    default:
        break;
    }
}

void TcpClientModule::HandleLoginResponse(ImPdu * pdu)
{
    auto loginDoneCallbackParam = std::make_shared<z::login::LoginMsgServerParam>();
    loginDoneCallbackParam->result = z::login::LOGIN_MSG_SERVER_FAILED;

    im::login::LoginRes loginRes;
    uint32_t seqNo = pdu->GetSeqNum();

    bool ret = loginRes.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength());
    if (!ret) {
        loginDoneCallback_(loginDoneCallbackParam);
        return;
    }
    uint32_t resultCode = loginRes.result_code();
    std::string resultMsg = loginRes.result_string();

    if (resultCode != 0) {
        LOG_ERROR("login failed! resultCode: %d, resultMsg: %s", resultCode, resultMsg.c_str());
        loginDoneCallback_(loginDoneCallbackParam);
        return;
    }
    
    loginDoneCallbackParam->result = z::login::LOGIN_MSG_SERVER_SUCCESS;
    loginDoneCallback_(loginDoneCallbackParam);

    im::base::UserInfo userInfo = loginRes.user_info();

    IPduPacketParse* pModule = z::login::GetLoginModule();
    pModule->OnPacket();

    z::net::netlib_register_timer(std::bind(&TcpClientModule::TimerCallback, 
        this, std::placeholders::_1, std::placeholders::_2), 100);

}

void TcpClientModule::TimerCallback(uint8_t msg, uint32_t handle)
{
    uint64_t curTick = z::utils::GetTickCount();
    OnTimer(curTick);
}

NAMESPACE_END(core)
NAMESPACE_END(z)
