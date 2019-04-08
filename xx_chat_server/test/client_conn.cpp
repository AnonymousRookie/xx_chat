/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ClientConn
 */

#include "stdafx.h"
#include <functional>
#include "client_conn.h"
#include "im_pdu_base.h"
#include "client.h"
#include "utils.h"
#include "defines.h"

static ConnMap g_client_conn;

ClientConn::ClientConn()
{

}

ClientConn::ClientConn(std::shared_ptr<Client> client)
{
    client_ = client;
}

ClientConn::~ClientConn()
{
    //printf("ClientConn::~ClientConn()\n");
}

net_handle_t ClientConn::Connect(const std::string& ip,
    uint16_t port,
    const std::string& username,
    const std::string& passwd)
{
    handle_ = z::net::netlib_connect(ip.c_str(), port,
        std::bind(&ImConnCallback, std::placeholders::_1, 
            std::placeholders::_2, (void*)&g_client_conn)
    );
    if (handle_ != NETLIB_INVALID_HANDLE) {
        g_client_conn.insert(std::make_pair(handle_, shared_from_this()));
    }
    return handle_;
}

void ClientConn::Close()
{
    if (handle_ != NETLIB_INVALID_HANDLE) {
        z::net::netlib_close(handle_);
        g_client_conn.erase(handle_);
    }
}

uint32_t ClientConn::Login(const std::string& username, const std::string& passwd)
{
    im::login::LoginReq msg;
    msg.set_username(username);
    msg.set_password(passwd);
    msg.set_online_status(im::base::UserStatType::USER_STATUS_ONLINE);

    ImPdu pdu;
    pdu.SetPBMsg(&msg);
    pdu.SetServiceId(im::base::ServiceID::SID_LOGIN);
    pdu.SetCommandId(im::base::LoginCmdID::CID_LOGIN_REQ_USERLOGIN);
    uint32_t seqNo = 0;
    pdu.SetSeqNum(seqNo);
    SendPdu(&pdu);
    return seqNo;
}

void ClientConn::OnConfirm()
{
    LOG_INFO("connect msg_server success.");
    auto spClient = client_.lock();
    if (spClient) {
        spClient->OnConnect();
    }
}

void ClientConn::OnClose()
{
    LOG_WARN("OnClose from handle: %d", handle_);
    Close();
}

void ClientConn::OnTimer(uint64_t currentTick)
{
    if (currentTick > lastSendTick_ + CLIENT_HEARTBEAT_INTERVAL) {
        ImPdu pdu;
        im::other::HeartBeat msg;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(im::base::ServiceID::SID_OTHER);
        pdu.SetCommandId(im::base::OtherCmdID::CID_OTHER_HEARTBEAT);
        uint32_t seqNo = 0;
        pdu.SetSeqNum(seqNo);
        SendPdu(&pdu);
    }
    if (currentTick > lastRecvTick_ + CLIENT_TIMEOUT) {
        LOG_WARN("conn to msg_server timeout!");
        Close();
    }
}

void ClientConn::HandlePdu(ImPdu* pdu)
{
    switch (pdu->GetCommandId()) {
    case im::base::OtherCmdID::CID_OTHER_HEARTBEAT:
        break;
    case im::base::LoginCmdID::CID_LOGIN_RES_USERLOGIN:
        HandleLoginResponse(pdu);
        break;
    default:
        LOG_WARN("Unknow msgType: %d", pdu->GetCommandId());
        break;
    }
}

void ClientConn::HandleLoginResponse(ImPdu* pdu)
{
    auto spClient = client_.lock();
    Z_CHECK(spClient);

    im::login::LoginRes loginRes;
    uint32_t seqNo = pdu->GetSeqNum();
    if (!loginRes.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength())) {
        spClient->OnError(seqNo, pdu->GetCommandId(), "Parse pb error!");
        return;
    }
    uint32_t resultCode = loginRes.result_code();
    std::string resultMsg = loginRes.result_string();

    if (resultCode == 0) {
        open_ = true;
        im::base::UserInfo userInfo = loginRes.user_info();
        spClient->OnLogin(seqNo, resultCode, resultMsg, &userInfo);
    }
    else {
        spClient->OnLogin(seqNo, resultCode, resultMsg);
    }
}
