/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: RouteServer
 */

#include "stdafx.h"
#include <vector>
#include <string>
#include "net_lib.h"
#include "config_file_reader.h"
#include "route_conn.h"
#include "string_util.h"

// this callback will be replaced by imconn_callback() in OnConnect()
void route_serv_callback(uint8_t msg, uint32_t handle)
{
	if (msg == NETLIB_MSG_CONNECT) {
        RouteConnPtr pConn = std::make_shared<RouteConn>();
        pConn->OnConnect(handle);
	} else {
		LOG_ERROR("error msg: %d", msg);
	}
}

int main(int argc, char** argv)
{
    signal(SIGPIPE, SIG_IGN);

    std::string path = z::utils::GetProgramAbsolutePath(argv[0]);

    Logger::GetInstance().SetFileBaseName((path + "/log/route_server").c_str());
    Logger::GetInstance().SetRollSize(10 * 1024 * 1024);
    Logger::GetInstance().Start();

    ConfigFileReader configFileReader(path + "/route_server.json");
    auto& dom = configFileReader.GetDom();
    if (!dom.IsObject()) {
        LOG_ERROR("parser route_server.json failed!");
        return -1;
    }

    auto& route_server_info = dom["route_server_info"];
    std::string listenIp = route_server_info["listen_ip"].GetString();
    uint16_t listenPort = route_server_info["listen_port"].GetUint();

    int ret = z::net::netlib_init();
    if (ret == NETLIB_ERROR) {
        return ret;
    }

    ret = z::net::netlib_listen(listenIp.c_str(), listenPort, 
        std::bind(&route_serv_callback, std::placeholders::_1, std::placeholders::_2));
    if (ret == NETLIB_ERROR) {
        return ret;
    }

    LOG_INFO("route server start listen on: %s:%d", listenIp.c_str(), listenPort);

    init_routeconn_timer_callback();
    z::net::netlib_eventloop();

    return 0;
}