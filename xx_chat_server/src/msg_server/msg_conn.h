/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: Client与MsgServer之间的连接
 */

#ifndef MSG_SERVER_MSG_CONN_H
#define MSG_SERVER_MSG_CONN_H

#include <string>
#include <memory>
#include "im_conn.h"

class MsgConn : public ImConn
{
public:
    MsgConn();
    virtual ~MsgConn();

public:
    std::string GetLoginName() { return loginName_; }
    uint32_t GetUserId() { return userId_; }
    void SetUserId(uint32_t user_id) { userId_ = user_id; }

    uint32_t GetHandle() { return handle_; }

    void SetOpen(bool open = true) { open_ = open; }
    bool IsOpen() { return open_; }

    void SetOnlineStatus(uint32_t status) { onlineStatus_ = status; }
    uint32_t GetOnlineStatus() { return onlineStatus_; }

    virtual void Close();
    virtual void OnConnect(net_handle_t handle);
    virtual void OnClose();
    virtual void OnTimer(uint64_t curr_tick);
    virtual void HandlePdu(std::shared_ptr<ImPdu> pPdu);

private:
    void HandleLoginRequest(std::shared_ptr<ImPdu> pdu);
    void HandleBuddyListAllUserRequest(std::shared_ptr<ImPdu> pdu);
    void HandleClientMsgData(std::shared_ptr<ImPdu> pdu);

private:
    std::string loginName_;
    uint32_t userId_;
    uint64_t loginTime_;
    bool open_;               // only db validate passed will be set to true
    uint32_t onlineStatus_;   // 1: on-line, 2: off-line
};

typedef std::shared_ptr<MsgConn> MsgConnPtr;
void init_msg_conn();


#endif  // MSG_SERVER_MSG_CONN_H