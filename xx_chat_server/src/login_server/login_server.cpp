/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: LoginServer
 */

#include "stdafx.h"
#include <vector>
#include <functional>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "net/event_loop.h"
#include "net/tcp_server.h"
#include "net/net_address.h"
#include "net/buffer.h"
#include "net_lib.h"
#include "config_file_reader.h"
#include "login_conn.h"
#include "http_conn.h"
#include "string_util.h"
#include "login_server.h"
#include "http_parser_wrapper.h"

bool LoginServer::init(const char* ip, short port, z::net::EventLoop* loop)
{
    z::net::NetAddress addr(ip, port);
    server_.reset(new z::net::TcpServer(loop, addr, "LoginServer"));
    server_->setConnectionCallback(std::bind(&LoginServer::onConnected, this, std::placeholders::_1));
    server_->start();

    return true;
}

void LoginServer::uninit()
{
    
}

void LoginServer::onConnected(std::shared_ptr<z::net::TcpConnection> conn)
{
    if (conn->connected()) {
        conn->setMessageCallback(std::bind(&LoginServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }
    else {
        onDisconnected(conn);
    }
}

void LoginServer::onDisconnected(const std::shared_ptr<z::net::TcpConnection>& conn)
{
    
}

void LoginServer::onMessage(const std::shared_ptr<z::net::TcpConnection>& conn, z::net::Buffer* buf, z::net::Timestamp time)
{
    std::string inbuf;
    inbuf.append(buf->peek(), buf->readableBytes());

    printf("%s\n", inbuf.c_str());

    if (z::utils::startWithPrefixString(inbuf, "GET")) {
        handleClientData(conn, buf->peek(), buf->readableBytes());
        conn->forceClose();
        return;
    }
    else {
        handleMsgServerData(conn, buf->peek(), buf->readableBytes());
    }
}

void LoginServer::handleClientData(const std::shared_ptr<z::net::TcpConnection>& conn, const char* data, int len)
{
    CHttpParserWrapper httpParser_;
    httpParser_.ParseHttpContent(data, len);
    if (httpParser_.IsReadAll()) {
        std::string url =  httpParser_.GetUrl();

        if (url.find("/msg_server") == std::string::npos) {
            LOG_WARN("url unknown, url=%s", url.c_str());
        }
        else {
            std::string content = httpParser_.GetBodyContent();

            // 没有可用的msg_server
            if (msgServerInfos_.empty()) {
                rapidjson::StringBuffer s;
                rapidjson::Writer<rapidjson::StringBuffer> writer(s);
                writer.StartObject();
                writer.Key("code");
                writer.Uint(1);
                writer.Key("msg");
                writer.String("no msg_server!");
                writer.Key("ip");
                writer.String("");
                writer.Key("port");
                writer.Uint(0);
                writer.EndObject();
                std::string strContent = s.GetString();

                char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
                snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, strContent.length(), strContent.c_str());
                conn->send((void*)szContent, strlen(szContent));

                delete[] szContent;
                return;
            }

            // 查找一个负载较小的msg_server给client使用
            int minUserCnt = INT_MAX;
            MsgServerInfo minUserMsgServerInfo;
            for (auto it = msgServerInfos_.begin() ; it != msgServerInfos_.end(); it++) {
                MsgServerInfo& msgServerInfo = *it;
                if ((msgServerInfo.cur_conn_cnt < msgServerInfo.max_conn_cnt) && (msgServerInfo.cur_conn_cnt < minUserCnt)) {
                    minUserCnt = msgServerInfo.cur_conn_cnt;
                    minUserMsgServerInfo = msgServerInfo;
                }
            }

            if (minUserCnt == INT_MAX) {
                LOG_WARN("all msg_server are full!");

                rapidjson::StringBuffer s;
                rapidjson::Writer<rapidjson::StringBuffer> writer(s);
                writer.StartObject();
                writer.Key("code");
                writer.Uint(2);
                writer.Key("msg");
                writer.String("all msg_server are full!");
                writer.Key("ip");
                writer.String("");
                writer.Key("port");
                writer.Uint(0);
                writer.EndObject();
                std::string strContent = s.GetString();

                char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
                snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, strContent.length(), strContent.c_str());

                conn->send((void*)szContent, strlen(szContent));

                delete[] szContent;
                return;
            } 
            else {
                rapidjson::StringBuffer s;
                rapidjson::Writer<rapidjson::StringBuffer> writer(s);
                writer.StartObject();
                writer.Key("code");
                writer.Uint(0);
                writer.Key("msg");
                writer.String("");
                writer.Key("ip");
                writer.String(minUserMsgServerInfo.ip_addr.c_str());
                writer.Key("port");
                writer.Uint(minUserMsgServerInfo.port);
                writer.EndObject();
                std::string strContent = s.GetString();

                char* szContent = new char[HTTP_RESPONSE_HTML_MAX];
                uint32_t nLen = strContent.length();
                snprintf(szContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, nLen, strContent.c_str());

                conn->send((void*)szContent, strlen(szContent));
                
                delete[] szContent;
                return;
            }
        }
    }
}

void LoginServer::handleMsgServerData(const std::shared_ptr<z::net::TcpConnection>& conn, const char* data, int len)
{
    
}

void ClientCallback(uint8_t msg, uint32_t handle)
{
    LOG_INFO("client come in");
    if (NETLIB_MSG_CONNECT == msg) {
        LoginConnPtr conn = std::make_shared<LoginConn>();
        conn->OnConnect2(handle, LOGIN_CONN_TYPE_CLIENT);
    } else {
        LOG_ERROR("error msg: %d", msg);
    }
}

void HttpClientCallback(uint8_t msg, uint32_t handle)
{
    LOG_INFO("http client come in");
    if (NETLIB_MSG_CONNECT == msg) {
        HttpConnPtr conn = std::make_shared<HttpConn>();
        conn->OnConnect(handle);
    } else {
        LOG_ERROR("error msg: %d", msg);
    }
}

void MsgServerCallback(uint8_t msg, uint32_t handle)
{
    LOG_INFO("msg server come in");
    if (NETLIB_MSG_CONNECT == msg) {
        LoginConnPtr conn = std::make_shared<LoginConn>();
        conn->OnConnect2(handle, LOGIN_CONN_TYPE_MSG_SERV);
    } else {
        LOG_ERROR("error msg: %d", msg);
    }
}

int main(int argc, char** argv)
{
    signal(SIGPIPE, SIG_IGN);

    std::string path = z::utils::GetProgramAbsolutePath(argv[0]);
    printf("%s\n", path.c_str());

    Logger::GetInstance().SetFileBaseName((path + "/log/login_server").c_str());
    Logger::GetInstance().SetRollSize(10 * 1024 * 1024);
    Logger::GetInstance().Start();

    ConfigFileReader configFileReader(path + "/login_server.json");
    auto& dom = configFileReader.GetDom();
    if (!dom.IsObject()) {
        LOG_ERROR("parser login_server.json failed!");
        return -1;
    }

    z::net::EventLoop loop;

    auto& server_for_client_conn = dom["server_for_client_conn"];
    auto& server_for_http_client_conn = dom["server_for_http_client_conn"];
    auto& server_for_msg_server_conn = dom["server_for_msg_server_conn"];

    std::string listenIpForClientConn = server_for_client_conn["listen_ip"].GetString();
    uint16_t listenPortForClientConn = server_for_client_conn["port"].GetUint();

    const char* listenIpForHttpClientConn = server_for_http_client_conn["listen_ip"].GetString();
    uint16_t listenPortForHttpClientConn = server_for_http_client_conn["port"].GetUint();

    std::string listenIpForMsgServerConn = server_for_msg_server_conn["listen_ip"].GetString();
    uint16_t listenPortForMsgServerConn = server_for_msg_server_conn["port"].GetUint();

    // int ret = z::net::netlib_init();
    // if (NETLIB_ERROR == ret) {
    //     return ret;
    // }

    LoginServer loginServer;
    loginServer.init(listenIpForHttpClientConn, listenPortForHttpClientConn, &loop);
    loop.loop();

    return 0;

/*
    ret = z::net::netlib_listen(listenIpForClientConn.c_str(), 
                                listenPortForClientConn,
                                std::bind(&ClientCallback, std::placeholders::_1, std::placeholders::_2));
    if (NETLIB_ERROR == ret) {
        return ret;
    }
    
    ret = z::net::netlib_listen(listenIpForHttpClientConn.c_str(), 
                                listenPortForHttpClientConn,
                                std::bind(&HttpClientCallback, std::placeholders::_1, std::placeholders::_2));
    if (NETLIB_ERROR == ret) {
        return ret;
    }

    ret = z::net::netlib_listen(listenIpForMsgServerConn.c_str(), 
                                listenPortForMsgServerConn,
                                std::bind(&MsgServerCallback, std::placeholders::_1, std::placeholders::_2));
    if (NETLIB_ERROR == ret) {
        return ret;
    }

    LOG_INFO("login server start listen on:\nFor client: %s:%u\nFor http client: %s:%u\nFor msg server: %s:%u\n",
             listenIpForClientConn.c_str(), listenPortForClientConn,
             listenIpForHttpClientConn.c_str(), listenPortForHttpClientConn,
             listenIpForMsgServerConn.c_str(), listenPortForMsgServerConn);

    init_login_conn();
    init_http_conn();

    z::net::netlib_eventloop();

    return 0;
*/
}