/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: MsgServer
 */

#include "stdafx.h"
#include <list>
#include <memory>
#include "net_lib.h"
#include "config_file_reader.h"
#include "login_server_conn.h"
#include "db_server_conn.h"
#include "route_server_conn.h"
#include "msg_conn.h"
#include "serv_info.h"
#include "string_util.h"

#define DEFAULT_CONCURRENT_DB_COUNT_CNT 10

// for client connect in
void msg_serv_callback(uint8_t msg, uint32_t handle)
{
    if (msg == NETLIB_MSG_CONNECT) {
        MsgConnPtr pConn = std::make_shared<MsgConn>();
        pConn->OnConnect(handle);
    } else {
        LOG_ERROR("error msg: %d", msg);
    }
}

int main(int argc, char** argv)
{
    signal(SIGPIPE, SIG_IGN);

    std::string path = z::utils::GetProgramAbsolutePath(argv[0]);

    Logger::GetInstance().SetFileBaseName((path + "/log/msg_server").c_str());
    Logger::GetInstance().SetRollSize(10 * 1024 * 1024);
    Logger::GetInstance().Start();

    ConfigFileReader configFileReader(path + "/msg_server.json");
    auto& dom = configFileReader.GetDom();
    
    // 监听client的连接
    auto& msg_server_info = dom["msg_server_info"];
    std::string listen_ip = msg_server_info["listen_ip"].GetString();
    uint16_t listen_port = msg_server_info["listen_port"].GetUint();
    std::string msg_server_ip_for_conn_login_server = msg_server_info["msg_server_ip_for_conn_login_server"].GetString();
    uint32_t max_conn_cnt = msg_server_info["max_conn_cnt"].GetUint();

    auto& db_server_info = dom["db_server_info"];
    ServerListType db_server_list;
    auto spDbServerInfo = std::make_shared<serv_info_t>();
    spDbServerInfo->server_ip = db_server_info["server_ip"].GetString();
    spDbServerInfo->server_port = db_server_info["server_port"].GetUint();
    db_server_list.push_back(spDbServerInfo);

    auto& login_server_info = dom["login_server_info"];
    ServerListType login_server_list;
    auto spLoginServerInfo = std::make_shared<serv_info_t>();
    spLoginServerInfo->server_ip = login_server_info["server_ip"].GetString();
    spLoginServerInfo->server_port = login_server_info["server_port"].GetUint();
    login_server_list.push_back(spLoginServerInfo);

    auto& route_server_info = dom["route_server_info"];
    ServerListType route_server_list;
    auto spRouteServerInfo = std::make_shared<serv_info_t>();
    spRouteServerInfo->server_ip = route_server_info["server_ip"].GetString();
    spRouteServerInfo->server_port = route_server_info["server_port"].GetUint();
    route_server_list.push_back(spRouteServerInfo);

    int ret = z::net::netlib_init();
    if (ret == NETLIB_ERROR) {
        return ret;
    }

    ret = z::net::netlib_listen(listen_ip.c_str(), listen_port, 
        std::bind(&msg_serv_callback, std::placeholders::_1, std::placeholders::_2));
 
    if (ret == NETLIB_ERROR) {
        return ret;
    }

    LOG_INFO("msg server start listen on: %s:%d", listen_ip.c_str(), listen_port);

    init_msg_conn();
    init_db_serv_conn(&db_server_list);
    init_login_serv_conn(&login_server_list, msg_server_ip_for_conn_login_server.c_str(), listen_port, max_conn_cnt);
    init_route_serv_conn(&route_server_list);

    z::net::netlib_eventloop();

    return 0;
}