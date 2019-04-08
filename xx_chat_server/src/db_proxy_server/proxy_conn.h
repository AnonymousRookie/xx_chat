/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: MsgServer与DbProxyServer之间的连接
 */

#ifndef DB_PROXY_SERVER_PROXY_CONN_H
#define DB_PROXY_SERVER_PROXY_CONN_H

#include <list>
#include <memory>
#include <mutex>
#include "im_conn.h"
#include "im_pdu_base.h"

typedef struct {
    uint32_t connUuid;
    ImPduPtr pdu;
} ResponsePdu;

typedef std::shared_ptr<ResponsePdu> ResponsePduPtr;

class ProxyConn : public ImConn
{
public:
    ProxyConn();
    virtual ~ProxyConn();

    virtual void Close();
    virtual void OnConnect(net_handle_t handle);
    virtual void OnRead();
    virtual void OnClose();
    virtual void OnTimer(uint64_t curr_tick);

    void HandlePduBuf(uchar_t* pdu_buf, uint32_t pdu_len);
    static void AddResponsePdu(uint32_t conn_uuid, ImPduPtr pdu);  // 工作线程调用
    static void SendResponsePduList();  // 主线程调用

    void SetUuid();

private:
    /**
     * 由于处理请求和发送回复在两个线程, 
     * socket的handle可能重复, 所以需要一个uuid来表示一个连接
     */
    static uint32_t uuid_alloctor_;
    uint32_t uuid_;

    static std::mutex mutex_;
    static std::list<ResponsePduPtr> response_pdu_list_;
};

typedef std::shared_ptr<ProxyConn> ProxyConnPtr;

int init_proxy_conn(uint32_t thread_num);
ProxyConnPtr get_proxy_conn_by_uuid(uint32_t uuid);


#endif  // DB_PROXY_SERVER_PROXY_CONN_H