/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: netlib网络处理模块
 */

#ifndef BASE_NETLIB_H
#define BASE_NETLIB_H

#include "types.h"
#include "defines.h"

#define NETLIB_OPT_GET_REMOTE_IP       1
#define NETLIB_OPT_GET_REMOTE_PORT     2
#define NETLIB_OPT_GET_LOCAL_IP        3
#define NETLIB_OPT_GET_LOCAL_PORT      4
#define NETLIB_OPT_SET_SEND_BUF_SIZE   5
#define NETLIB_OPT_SET_RECV_BUF_SIZE   6

#define NETLIB_MAX_SOCKET_BUF_SIZE     (128 * 1024)

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

int netlib_init();

int netlib_destroy();

int netlib_listen(const char* server_ip, uint16_t port, const NewConnectionCallback& callback);

net_handle_t netlib_connect(const char* server_ip, uint16_t port, const MessageCallback& callback);

int netlib_send(net_handle_t handle, void* buf, int len);

int netlib_recv(net_handle_t handle, void* buf, int len);

int netlib_close(net_handle_t handle);

int netlib_option(net_handle_t handle, int opt, void* optval);

int netlib_register_message_callback(net_handle_t handle, const MessageCallback& cb);

int netlib_register_timer(const TimerCallback& callback, uint64_t interval);

int netlib_delete_timer(const TimerCallback& callback);

int netlib_add_loop(const LoopCallback& callback);

void netlib_eventloop(uint32_t wait_timeout = 100);

void netlib_start_event();

void netlib_stop_event();

bool netlib_is_running();

NAMESPACE_END(net)
NAMESPACE_END(z)

#endif  // BASE_NETLIB_H
