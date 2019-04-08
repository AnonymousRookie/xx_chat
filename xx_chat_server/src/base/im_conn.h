/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 所有连接对象的基类ImConn
 */

#ifndef BASE_IM_CONN_H
#define BASE_IM_CONN_H

#include <unordered_map>
#include <memory>
#include "net_lib.h"
#include "util_pdu.h"
#include "im_pdu_base.h"

#define SERVER_HEARTBEAT_INTERVAL 5000
#define SERVER_TIMEOUT 30000
#define CLIENT_HEARTBEAT_INTERVAL 30000
#define CLIENT_TIMEOUT 120000
#define READ_BUF_SIZE 2048


class ImConn : public std::enable_shared_from_this<ImConn>
{
public:
    ImConn();
    virtual ~ImConn();

    bool IsBusy() { return busy_; }
    int SendPdu(ImPduPtr pdu);
    int SendPdu(ImPdu* pdu);
    int Send(void* data, int len);

    virtual void OnConnect(net_handle_t handle) { handle_ = handle; }
    virtual void OnConfirm() {}
    virtual void OnRead();
    virtual void OnWrite();
    virtual void OnClose() {}
    virtual void OnTimer(uint64_t curr_tick) {}
    virtual void OnWriteCompelete() {}

    virtual void HandlePdu(ImPdu* pPdu) {}

protected:
    net_handle_t handle_;
    bool busy_;

    std::string peerIp_;
    uint16_t peerPort_;

    SimpleBuffer inBuf_;
    SimpleBuffer outBuf_;

    uint32_t recvBytes_;

    uint64_t lastSendTick_;
    uint64_t lastRecvTick_;
    uint64_t lastAllUserTick_;
}; 

typedef std::shared_ptr<ImConn> ImConnPtr;
typedef std::unordered_map<net_handle_t, ImConnPtr> ConnMap;
typedef std::unordered_map<uint32_t, ImConnPtr> UserMap;

void ImConnCallback(uint8_t msg, uint32_t handle, void* callbackData);

#endif  // BASE_IM_CONN_H