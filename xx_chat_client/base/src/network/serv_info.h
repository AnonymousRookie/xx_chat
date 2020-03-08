/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: serv_info基本操作封装
 */

#ifndef BASE_SERV_INFO_H
#define BASE_SERV_INFO_H

#include <string>
#include <vector>
#include <memory>
#include "im_conn.h"
#include "utils.h"
#include "defines.h"

#define MAX_RECONNECT_CNT  64
#define MIN_RECONNECT_CNT  4

typedef struct {
    std::string server_ip;
    uint16_t server_port;
    uint32_t idle_cnt;
    uint32_t reconnect_cnt;
    std::shared_ptr<ImConn> serv_conn;
} serv_info_t;

typedef std::vector<std::shared_ptr<serv_info_t>> ServerListType;

template <class T>
void serv_init(ServerListType* server_list)
{
    Z_CHECK(server_list);
    for (uint32_t i = 0; i < server_list->size(); ++i) {
        std::shared_ptr<T> conn = std::make_shared<T>();
        auto server = server_list->at(i);
        conn->Connect(server->server_ip.c_str(), server->server_port, i);
        server->serv_conn = conn;
        server->idle_cnt = 0;
        server->reconnect_cnt = MIN_RECONNECT_CNT / 2;
    }
}

template <class T>
void serv_check_reconnect(ServerListType* server_list)
{
    Z_CHECK(server_list);
    for (uint32_t i = 0; i < server_list->size(); ++i) {
        auto server = server_list->at(i);
        std::shared_ptr<T> conn = std::dynamic_pointer_cast<T>(server->serv_conn);
        if (!conn) {
            if (server->idle_cnt++ > server->reconnect_cnt) {
                conn = std::make_shared<T>();
                conn->Connect(server->server_ip.c_str(), server->server_port, i);
                server->serv_conn = conn;
            }
        }
    }
}

template <class T>
void serv_reset(ServerListType* server_list, uint32_t serv_index)
{
    Z_CHECK(server_list);
    if (serv_index >= server_list->size()) {
        return;
    }

    auto server = server_list->at(serv_index);
    server->idle_cnt = 0;
    server->reconnect_cnt *= 2;
    server->serv_conn = NULL;
    if (server->reconnect_cnt > MAX_RECONNECT_CNT) {
        server->reconnect_cnt = MAX_RECONNECT_CNT;
    }
}


#endif  // BASE_SERV_INFO_H