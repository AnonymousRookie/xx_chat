/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: LoginServer与MsgServer之间的连接
 */

#ifndef LOGIN_SERVERR_LOGIN_CONN_H
#define LOGIN_SERVERR_LOGIN_CONN_H

#include <string>
#include <memory>
#include "im_conn.h"
#include "types.h"

enum {
    LOGIN_CONN_TYPE_CLIENT = 1,
    LOGIN_CONN_TYPE_MSG_SERV
};

typedef struct {
    std::string ip_addr;
    uint16_t	port;
    uint32_t	max_conn_cnt;
    uint32_t	cur_conn_cnt;
    std::string hostname;
} msg_serv_info_t;


class LoginConn : public ImConn
{
public:
    LoginConn();
    virtual ~LoginConn();

    void OnConnect2(net_handle_t handle, int conn_type);

    virtual void Close();
    virtual void OnClose();
    virtual void OnTimer(uint64_t curr_tick);
    virtual void HandlePdu(std::shared_ptr<ImPdu> pPdu);

private:
    void HandleMsgServInfo(std::shared_ptr<ImPdu> pPdu);
    void HandleUserCntUpdate(std::shared_ptr<ImPdu> pPdu);
    void HandleMsgServRequest(std::shared_ptr<ImPdu> pPdu);

private:
    int connType_;
};

typedef std::shared_ptr<LoginConn> LoginConnPtr;

void init_login_conn();


#endif  // LOGIN_SERVERR_LOGIN_CONN_H