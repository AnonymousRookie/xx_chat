/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: Client
 */

#ifndef TEST_CLIENT_CLIENT_H
#define TEST_CLIENT_CLIENT_H

#include <memory>
#include <string>
#include "types.h"
#include "im.base.pb.h"

class ClientConn;
class Client : public std::enable_shared_from_this<Client>
{
public:
    Client(const std::string& name, const std::string& passwd, const std::string& domain);
    ~Client();

public:
    void Connect();
    void Close();
    uint32_t Login(const std::string& name, const std::string& passwd);

public:
    void TimerCallback(uint8_t msg, uint32_t handle);

public:
    void OnError(uint32_t seqNo, uint32_t resultCode, const std::string& resultMsg);
    void OnConnect();
    void OnClose();
    void OnLogin(uint32_t seqNo, uint32_t resultCode, const std::string& resultMsg, im::base::UserInfo* userInfo = NULL);

private:
    std::string name_;
    std::string passwd_;
    std::string loginDomain_;
    net_handle_t handle_;

    im::base::UserInfo selfInfo_;

    std::shared_ptr<ClientConn> clientConn_;
    bool logined_{ false };
};


#endif  // TEST_CLIENT_CLIENT_H