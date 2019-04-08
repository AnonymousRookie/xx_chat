/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ClientConn
 */

#ifndef TEST_CLIENT_CLIENT_CONN_H
#define TEST_CLIENT_CLIENT_CONN_H

#include <string>
#include "im_conn.h"
#include "im.base.pb.h"
#include "im.login.pb.h"
#include "im.message.pb.h"
#include "im.other.pb.h"
#include <memory>
#include "im.server.pb.h"

class ImPdu;
class Client;
class ClientConn : public ImConn
{
public:
    ClientConn();
    ClientConn(std::shared_ptr<Client> client);
    virtual ~ClientConn();

    bool IsOpen() { return open_; }
    net_handle_t Connect(const std::string& ip, 
                         uint16_t port, 
                         const std::string& username, 
                         const std::string& passwd);

    virtual void Close();

public:
    uint32_t Login(const std::string& username, const std::string& passwd);

public:
    virtual void OnConfirm();
    virtual void OnClose();
    virtual void OnTimer(uint64_t currentTick);
    virtual void HandlePdu(ImPdu* pdu);

private:
    void HandleLoginResponse(ImPdu* pdu);

private:
    bool open_{ false };
    std::weak_ptr<Client> client_;
};



#endif  // TEST_CLIENT_CLIENT_CONN_H