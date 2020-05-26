/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <memory>
#include "defines.h"
#include "types.h"
#include "network\im_pdu_base.h"
#include "network\im_conn.h"

NAMESPACE_BEGIN(im)
NAMESPACE_BEGIN(login)

class LoginRes;

NAMESPACE_END(login)
NAMESPACE_END(im)


NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

class ITcpClientModule
{
public:
    virtual net_handle_t DoLogin(
        const std::string& ip, 
        uint16_t port,
        const std::string& username,
        const std::string& password,
        std::function<void(std::shared_ptr<void>)> loginDoneCallback) = 0;

    // ����Э���
    virtual void SendPacket(ImPdu* pdu) {}
    virtual void SendPacket(uint16_t serviceId, uint16_t cmdId, google::protobuf::MessageLite* pbMsg) = 0;
};

std::shared_ptr<ITcpClientModule> GetTcpClientModule();

typedef std::function<void(std::shared_ptr<void>)> LoginDoneCallback;

class TcpClientModule final : public ITcpClientModule
    , public ImConn
{
public:
    TcpClientModule();
    virtual ~TcpClientModule();

    virtual net_handle_t DoLogin(
        const std::string& ip,
        uint16_t port,
        const std::string& username,
        const std::string& password,
        std::function<void(std::shared_ptr<void>)> loginDoneCallback);

    virtual void SendPacket(uint16_t serviceId, uint16_t cmdId, google::protobuf::MessageLite* pbMsg);

    virtual void Close();
    virtual void TimerCallback(uint8_t msg, uint32_t handle);

public:
    virtual void OnConfirm();
    virtual void OnClose();
    virtual void OnTimer(uint64_t currentTick);
    virtual void HandlePdu(std::shared_ptr<ImPdu> pdu);

private:
    void HandleLoginResponse(std::shared_ptr<ImPdu> pdu);
    void HandlePackets(std::shared_ptr<ImPdu> pdu);

private:
    im::login::LoginRes* loginResp_;
    uint8_t tcpClientState_;

    std::string username_;
    std::string password_;

    LoginDoneCallback loginDoneCallback_;
};


NAMESPACE_END(core)
NAMESPACE_END(z)
