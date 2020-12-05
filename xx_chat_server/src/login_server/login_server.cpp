/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: LoginServer
 */

#include "stdafx.h"
#include <vector>
#include <functional>
#include "net_lib.h"
#include "config_file_reader.h"
#include "login_conn.h"
#include "http_conn.h"
#include "string_util.h"


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

    Logger::GetInstance().SetFileBaseName((path + "/log/login_server").c_str());
    Logger::GetInstance().SetRollSize(10 * 1024 * 1024);
    Logger::GetInstance().Start();

    ConfigFileReader configFileReader(path + "/login_server.json");
    auto& dom = configFileReader.GetDom();
    auto& server_for_client_conn = dom["server_for_client_conn"];
    auto& server_for_http_client_conn = dom["server_for_http_client_conn"];
    auto& server_for_msg_server_conn = dom["server_for_msg_server_conn"];

    std::string listenIpForClientConn = server_for_client_conn["listen_ip"].GetString();
    uint16_t listenPortForClientConn = server_for_client_conn["port"].GetUint();
    std::string listenIpForHttpClientConn = server_for_http_client_conn["listen_ip"].GetString();
    uint16_t listenPortForHttpClientConn = server_for_http_client_conn["port"].GetUint();
    std::string listenIpForMsgServerConn = server_for_msg_server_conn["listen_ip"].GetString();
    uint16_t listenPortForMsgServerConn = server_for_msg_server_conn["port"].GetUint();

    int ret = z::net::netlib_init();
    if (NETLIB_ERROR == ret) {
        return ret;
    }

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
}