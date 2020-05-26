/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: MsgServer与DbProxyServer之间的连接
 */

#ifndef MSG_SERVER_DB_SERVER_CONN_H
#define MSG_SERVER_DB_SERVER_CONN_H

#include <memory>
#include "im_conn.h"
#include "serv_info.h"

class DBServConn : public ImConn
{
public:
    DBServConn();
    virtual ~DBServConn();

    bool IsOpen() { return opened_; }
    void Connect(const char* serverIp, uint16_t serverPort, uint32_t servIndex);
    virtual void Close();

    virtual void OnConfirm();
    virtual void OnClose();
    virtual void OnTimer(uint64_t currTick);
    virtual void HandlePdu(std::shared_ptr<ImPdu> pdu);

private:
    void HandleLoginValidResponse(std::shared_ptr<ImPdu> pdu);
    void HandleBuddyListAllUserResponse(std::shared_ptr<ImPdu> pdu);

private:
    bool opened_;
    uint32_t serverIndex_;
};

typedef std::shared_ptr<DBServConn> DBServConnPtr;

void init_db_serv_conn(ServerListType* server_list);
DBServConnPtr get_db_serv_conn_for_login();
DBServConnPtr get_db_serv_conn();

#endif  // MSG_SERVER_DB_SERVER_CONN_H