/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 数据库代理服务器
 */

#include "stdafx.h"
#include <string>
#include "net_lib.h"
#include "config_file_reader.h"
#include "db_pool.h"
#include "proxy_conn.h"
#include "string_util.h"

void proxy_serv_callback(uint8_t msg, uint32_t handle)
{
    if (NETLIB_MSG_CONNECT == msg) {
        ProxyConnPtr conn = std::make_shared<ProxyConn>();
        conn->SetUuid();
        conn->OnConnect(handle);
    } else {
        LOG_ERROR("error msg: %d", msg);
    }
}

int main(int argc, char** argv)
{
    signal(SIGPIPE, SIG_IGN);

    std::string path = z::utils::GetProgramAbsolutePath(argv[0]);

    Logger::GetInstance().SetFileBaseName((path + "/log/db_proxy_server").c_str());
    Logger::GetInstance().SetRollSize(10 * 1024 * 1024);
    Logger::GetInstance().Start();

    ConfigFileReader configFileReader(path + "/db_proxy_server.json");
    auto& dom = configFileReader.GetDom();
        if (!dom.IsObject()) {
        LOG_ERROR("parser route_server.json failed!");
        return -1;
    }

    auto& db_proxy_server = dom["db_proxy_server"];
    std::string listenIp = db_proxy_server["listen_ip"].GetString();
    uint16_t listenPort = db_proxy_server["port"].GetUint();
    uint32_t threadNum = db_proxy_server["thread_num"].GetUint();

    auto pDBManager = DBManager::GetInstance();
    if (!pDBManager) {
        return -1;
    }

    int ret = z::net::netlib_init();
    if (NETLIB_ERROR == ret) {
        return ret;
    }

    // 启动任务队列, 用于处理任务
    init_proxy_conn(threadNum);

    ret = z::net::netlib_listen(listenIp.c_str(), listenPort, 
        std::bind(&proxy_serv_callback, std::placeholders::_1, std::placeholders::_2));

    if (NETLIB_ERROR == ret) {
        return ret;
    }

    LOG_INFO("db_proxy_server listen on: %s:%d", listenIp.c_str(), listenPort);

    enum { waitTimeout = 10 };
    z::net::netlib_eventloop(waitTimeout);

    return 0;
}