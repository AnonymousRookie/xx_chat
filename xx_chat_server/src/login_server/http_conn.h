/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: Client与LoginServer之间的Http连接
 */

#ifndef LOGIN_SERVERR_HTTP_CONN_H
#define LOGIN_SERVERR_HTTP_CONN_H

#include <unordered_map>
#include <map>
#include <memory>
#include <string>
#include "net_lib.h"
#include "utils.h"
#include "http_parser_wrapper.h"
#include "util_pdu.h"

#define HTTP_CONN_TIMEOUT 60000
#define READ_BUF_SIZE 2048
#define HTTP_RESPONSE_HTML_MAX 1024
#define HTTP_RESPONSE_HTML "HTTP/1.1 200 OK\r\n"  \
                           "Connection:close\r\n" \
                           "Content-Length:%d\r\n"\
                           "Content-Type:text/html;charset=utf-8\r\n\r\n%s"


enum {
    CONN_STATE_IDLE,
    CONN_STATE_CONNECTED,
    CONN_STATE_OPEN,
    CONN_STATE_CLOSED,
};

class HttpConn : public std::enable_shared_from_this<HttpConn>
{
public:
    HttpConn();
    virtual ~HttpConn();

    uint32_t GetConnHandle() { return connHandle_; }
    char* GetPeerIP() { return (char*)peerIp_.c_str(); }

    int Send(void* data, int len);

    void Close();
    void OnConnect(net_handle_t handle);
    void OnRead();
    void OnWrite();
    void OnClose();
    void OnTimer(uint64_t curr_tick);
    void OnWriteComlete();

private:
    void HandleMsgServRequest(std::string& url, std::string& post_data);

protected:
    net_handle_t sockHandle_;
    uint32_t connHandle_;
    bool busy_;

    uint32_t state_;
    std::string peerIp_;
    uint16_t peerPort_;

    SimpleBuffer inBuf_;
    SimpleBuffer outBuf_;

    uint64_t lastSendTick_;
    uint64_t lastRecvTick_;

    CHttpParserWrapper httpParser_;
};

typedef std::shared_ptr<HttpConn> HttpConnPtr;
typedef std::unordered_map<uint32_t, HttpConnPtr> HttpConnMap;

HttpConnPtr FindHttpConnByHandle(uint32_t handle);
void init_http_conn();

#endif  // LOGIN_SERVERR_HTTP_CONN_H