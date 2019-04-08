/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: Client
 */

#include "stdafx.h"
#include <functional>
#include "client.h"
#include "client_conn.h"
#include "utils.h"
#include "http_client.h"
#include "rapidjson/document.h"

Client::Client(const std::string& name, const std::string& passwd, const std::string& domain)
    : name_(name)
    , passwd_(passwd)
    , loginDomain_(domain)
{

}

Client::~Client()
{
    //printf("Client::~Client()\n");
}

void Client::Connect()
{
    HttpClient httpClient;
    std::string url(loginDomain_ + "/msg_server");

    std::string response("");
    CURLcode ret = httpClient.Get(url, &response);
    if (ret != CURLE_OK) {
        LOG_ERROR("login failed! url: %s", url.c_str());
        return;
    }

    rapidjson::Document document;
    document.Parse(response.c_str());

    std::string ip("");
    uint32_t port = 0;

    try {
        uint32_t code = document["code"].GetUint();
        if (code != 0) {
            std::string msg = document["msg"].GetString();
            LOG_WARN("login failed! err: %s", msg.c_str());
            return;
        }
        ip = document["ip"].GetString();
        port = document["port"].GetUint();

    } catch (std::runtime_error err) {
        LOG_ERROR("login failed! parse json err: %s", response.c_str());
        return;
    }

    clientConn_ = std::make_shared<ClientConn>(shared_from_this());
  
    handle_ = clientConn_->Connect(ip.c_str(), port, name_, passwd_);
    if (handle_ == INVALID_SOCKET) {
        LOG_ERROR("connect msg_server failed!");
        return;
    }
    z::net::netlib_register_timer(
        std::bind(&Client::TimerCallback, this, std::placeholders::_1, 
            std::placeholders::_2), 1000
    );
}

void Client::Close()
{
}

uint32_t Client::Login(const std::string& name, const std::string& passwd)
{
    return clientConn_->Login(name, passwd);
}

void Client::TimerCallback(uint8_t msg, uint32_t handle)
{
    if (logined_) {
        uint64_t currentTick = z::utils::GetTickCount();
        clientConn_->OnTimer(currentTick);
    }
}

void Client::OnError(uint32_t seqNo, uint32_t resultCode, const std::string& resultMsg)
{
    LOG_ERROR("Error: %d, msg: %s", resultCode, resultMsg.c_str());
}

void Client::OnConnect()
{
    Login(name_, passwd_);
}

void Client::OnClose()
{
}

void Client::OnLogin(uint32_t seqNo, uint32_t resultCode, const std::string& resultMsg, im::base::UserInfo* userInfo /*= NULL*/)
{
    if (resultCode != 0) {
        LOG_ERROR("login failed! resultCode: %d, resultMsg: %s", resultCode, resultMsg.c_str());
        return;
    }
    if (!userInfo) {
        LOG_ERROR("userInfo is null!");
        return;
    }
    selfInfo_ = *userInfo;
    logined_ = true;
    LOG_INFO("login success!");
}
