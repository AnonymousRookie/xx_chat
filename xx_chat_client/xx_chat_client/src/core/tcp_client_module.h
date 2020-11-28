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
#include "..\core\module_base.h"

NAMESPACE_BEGIN(im)
NAMESPACE_BEGIN(login)

class LoginRes;

NAMESPACE_END(login)
NAMESPACE_END(im)


NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

enum TcpClientState 
{
    TcpClientState_Invalid = 0,
    TcpClientState_Connected,
    TcpClientState_DisConnected,
};

class TimerEvent;

class ITcpClientModule : public z::core::ModuleBase
{
public:
    virtual net_handle_t DoLogin(
        const std::string& ip, 
        uint16_t port,
        const std::string& username,
        const std::string& password,
        std::function<void(std::shared_ptr<void>)> loginDoneCallback) = 0;

    // 发送协议包
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

public:
    virtual void OnConfirm();
    virtual void OnClose();
    virtual void HandlePdu(std::shared_ptr<ImPdu> pdu);

public:
    // 定时发送心跳消息
    void StartSendHeartBeatMsg();
    void StopSendHeartBeatMsg();

    uint8_t GetTcpClientState();
    void SetTcpClientState(uint8_t state);

private:
    void HandleLoginResponse(std::shared_ptr<ImPdu> pdu);
    void HandlePackets(std::shared_ptr<ImPdu> pdu);

private:
    im::login::LoginRes* loginResp_;
    uint8_t tcpClientState_;

    std::string username_;
    std::string password_;

    uint8_t TcpClientState_ = TcpClientState_Invalid;

    LoginDoneCallback loginDoneCallback_;
    std::shared_ptr<TimerEvent> timerEvent_ = nullptr;
};


NAMESPACE_END(core)
NAMESPACE_END(z)
