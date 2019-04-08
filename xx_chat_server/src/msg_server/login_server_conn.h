/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: MsgServer与LoginServer之间的连接
 */

#ifndef MSG_SERVER_LOGIN_SERVER_CONN_H
#define MSG_SERVER_LOGIN_SERVER_CONN_H

#include <string>
#include <memory>
#include "serv_info.h"
#include "im_conn.h"

class ImPdu;

class LoginServConn : public ImConn
{
public:
    LoginServConn();
    virtual ~LoginServConn();

    bool IsOpen() { return opened_; }
    void Connect(const char* server_ip, uint16_t server_port, uint32_t serv_idx);

    virtual void Close();
    virtual void OnConfirm();
    virtual void OnClose();
    virtual void OnTimer(uint64_t curr_tick);
    virtual void HandlePdu(ImPdu* pPdu);

private:
    bool opened_;
    uint32_t serverIndex_;
};

typedef std::shared_ptr<LoginServConn> LoginServConnPtr;

void init_login_serv_conn(ServerListType* server_list, 
                          const char* msg_server_ip_addr,
                          uint16_t msg_server_port, 
                          uint32_t max_conn_cnt);

bool is_login_server_available();
void send_to_all_login_server(ImPdu* pPdu);


#endif  // MSG_SERVER_LOGIN_SERVER_CONN_H