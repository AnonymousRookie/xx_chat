/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: Client与LoginServer之间的Http连接
 */

#include "stdafx.h"
#include <cstring>
#include <cstdlib>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "http_conn.h"
#include "login_conn.h"
#include "http_parser_wrapper.h"


static HttpConnMap g_http_conn_map;
extern std::map<uint32_t, msg_serv_info_t*>  g_msg_serv_info;
// conn_handle从0开始递增，可以防止因socket handle重用引起的一些冲突
static uint32_t g_conn_handle_generator = 0;

HttpConnPtr FindHttpConnByHandle(uint32_t conn_handle)
{
    auto it = g_http_conn_map.find(conn_handle);
    if (it != g_http_conn_map.end()) {
        return it->second;
    }
    return nullptr;
}

void httpconn_callback(uint8_t msg, uint32_t handle, void* callback_data)
{
    uint32_t conn_handle = *((uint32_t*)(callback_data));
    HttpConnPtr conn = FindHttpConnByHandle(conn_handle);
    Z_CHECK(conn);

    switch (msg)
    {
    case NETLIB_MSG_READ:
        conn->OnRead();
        break;
    case NETLIB_MSG_WRITE:
        conn->OnWrite();
        break;
    case NETLIB_MSG_CLOSE:
        conn->OnClose();
        break;
    default:
        break;
    }
}

void http_conn_timer_callback(uint8_t msg, uint32_t handle)
{
    uint64_t cur_time = z::utils::GetTickCount();
    for (auto it = g_http_conn_map.begin(); it != g_http_conn_map.end(); ) {
        auto it_old = it;
        ++it;
        auto pConn = it_old->second;
        pConn->OnTimer(cur_time);
    }
}

void init_http_conn()
{
    z::net::netlib_register_timer(
        std::bind(&http_conn_timer_callback, std::placeholders::_1, 
            std::placeholders::_2), 1000
    );
}

HttpConn::HttpConn()
{
    busy_ = false;
    sockHandle_ = NETLIB_INVALID_HANDLE;
    state_ = CONN_STATE_IDLE;

    lastSendTick_ = z::utils::GetTickCount();
    lastRecvTick_ = z::utils::GetTickCount();

    connHandle_ = ++g_conn_handle_generator;
    if (connHandle_ == 0) {
        connHandle_ = ++g_conn_handle_generator;
    }
}

HttpConn::~HttpConn()
{

}

int HttpConn::Send(void* data, int len)
{
    lastSendTick_ = z::utils::GetTickCount();

    if (busy_) {
        outBuf_.Write(data, len);
        return len;
    }

    int ret = z::net::netlib_send(sockHandle_, data, len);
    if (ret < 0) {
        ret = 0;
    }

    if (ret < len) {
        outBuf_.Write((char*)data + ret, len - ret);
        busy_ = true;
    } else {
        OnWriteComlete();
    }

    return len;
}

void HttpConn::Close()
{
    state_ = CONN_STATE_CLOSED;
    g_http_conn_map.erase(connHandle_);
    z::net::netlib_close(sockHandle_);
}

void HttpConn::OnConnect(net_handle_t handle)
{
    LOG_INFO("HttpConn OnConnect, handle=%d", handle);
    sockHandle_ = handle;
    state_ = CONN_STATE_CONNECTED;
    g_http_conn_map.insert(std::make_pair(connHandle_, shared_from_this()));

    z::net::netlib_register_message_callback(
        handle,
        std::bind(&httpconn_callback, std::placeholders::_1, std::placeholders::_2,
        (void*)&connHandle_)
    );
    
    z::net::netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void*)&peerIp_);
}

void HttpConn::OnRead()
{
    for (;;) {
        uint32_t free_buf_len = inBuf_.GetAllocSize() - inBuf_.GetWriteOffset();
        if (free_buf_len < READ_BUF_SIZE + 1) {
            inBuf_.Extend(READ_BUF_SIZE + 1);
        }
        int ret = z::net::netlib_recv(sockHandle_, inBuf_.GetBuffer() + inBuf_.GetWriteOffset(), READ_BUF_SIZE);
        if (ret <= 0) {
            break;
        }
        inBuf_.IncWriteOffset(ret);
        lastRecvTick_ = z::utils::GetTickCount();
    }

    // 每次请求对应一个HTTP连接, 所以读完数据后, 不用在同一个连接里面准备读取下个请求
    char* in_buf = (char*)inBuf_.GetBuffer();
    uint32_t buf_len = inBuf_.GetWriteOffset();
    in_buf[buf_len] = '\0';

    // 如果buf_len 过长可能是受到攻击, 则断开连接
    // 正常的url最大长度为2048, 我们接受的所有数据长度不得大于1K
    if(buf_len > 1024) {
        LOG_WARN("get too much data:%s", in_buf);
        Close();
        return;
    }

    LOG_INFO("OnRead, buf_len=%u, conn_handle=%u", buf_len, connHandle_);

    httpParser_.ParseHttpContent(in_buf, buf_len);
    if (httpParser_.IsReadAll()) {
        std::string url =  httpParser_.GetUrl();
        if (strncmp(url.c_str(), "/msg_server", 11) == 0) {
            std::string content = httpParser_.GetBodyContent();
            HandleMsgServRequest(url, content);
        } else {
            LOG_WARN("url unknown, url=%s", url.c_str());
            Close();
        }
    }
}

void HttpConn::OnWrite()
{
    if (!busy_) {
        return;
    }
    int ret = z::net::netlib_send(sockHandle_, outBuf_.GetBuffer(), outBuf_.GetWriteOffset());
    if (ret < 0) {
        ret = 0;
    }
    int out_buf_size = (int)outBuf_.GetWriteOffset();
    outBuf_.Read(NULL, ret);
    if (ret < out_buf_size) {
        busy_ = true;
        LOG_WARN("not send all, remain=%d", outBuf_.GetWriteOffset());
    }
    else {
        OnWriteComlete();
        busy_ = false;
    }
}

void HttpConn::OnClose()
{
    Close();
}

void HttpConn::OnTimer(uint64_t currTick)
{
    if (currTick > lastRecvTick_ + HTTP_CONN_TIMEOUT) {
        LOG_WARN("HttpConn timeout, handle=%d", connHandle_);
        Close();
    }
}

void HttpConn::HandleMsgServRequest(std::string& url, std::string& post_data)
{
    msg_serv_info_t* pMsgServInfo;
    uint32_t min_user_cnt = (uint32_t)-1;
    auto it_min_conn = g_msg_serv_info.end();

    if(g_msg_serv_info.size() <= 0) {

        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);
        writer.StartObject();
        writer.Key("code");
        writer.Uint(1);
        writer.Key("msg");
        writer.String("没有msg_server");
        writer.Key("ip");
        writer.String("");
        writer.Key("port");
        writer.Uint(0);
        writer.EndObject();
        std::string strContent = s.GetString();

        char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
        snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, strContent.length(), strContent.c_str());
        Send((void*)szContent, strlen(szContent));
        delete[] szContent;
        return;
    }

    for (auto it = g_msg_serv_info.begin() ; it != g_msg_serv_info.end(); it++) {
        pMsgServInfo = it->second;
        if ( (pMsgServInfo->cur_conn_cnt < pMsgServInfo->max_conn_cnt) &&
            (pMsgServInfo->cur_conn_cnt < min_user_cnt)) {
            it_min_conn = it;
            min_user_cnt = pMsgServInfo->cur_conn_cnt;
        }
    }

    if (it_min_conn == g_msg_serv_info.end()) {
        LOG_WARN("All tcp msg_server are full!");

        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);
        writer.StartObject();
        writer.Key("code");
        writer.Uint(2);
        writer.Key("msg");
        writer.String("负载过高");
        writer.Key("ip");
        writer.String("");
        writer.Key("port");
        writer.Uint(0);
        writer.EndObject();
        std::string strContent = s.GetString();

        char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
        snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, strContent.length(), strContent.c_str());
        Send((void*)szContent, strlen(szContent));
        delete[] szContent;
        return;
    } else {
        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);
        writer.StartObject();
        writer.Key("code");
        writer.Uint(0);
        writer.Key("msg");
        writer.String("");
        writer.Key("ip");
        writer.String(it_min_conn->second->ip_addr.c_str());
        writer.Key("port");
        writer.Uint(it_min_conn->second->port);
        writer.EndObject();
        std::string strContent = s.GetString();

        char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
        uint32_t nLen = strContent.length();
        snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, nLen, strContent.c_str());
        Send((void*)szContent, strlen(szContent));
        delete[] szContent;
        return;
    }
}

void HttpConn::OnWriteComlete()
{
    LOG_INFO("write complete");
    Close();
}
