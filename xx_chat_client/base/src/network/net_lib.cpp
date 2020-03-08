/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: netlib网络处理模块
 */

#include "net_lib.h"
#include "base_socket.h"
#include "event_dispatch.h"
#include <thread>

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(net)

int netlib_init()
{
    int ret = NETLIB_OK;

#ifdef _WIN32
    WSADATA wsaData;
    WORD wReqest = MAKEWORD(1, 1);
    if (WSAStartup(wReqest, &wsaData) != 0) {
        ret = NETLIB_ERROR;
    }
#endif

    return ret;
}

int netlib_destroy()
{
    int ret = NETLIB_OK;

#ifdef _WIN32
    if (WSACleanup() != 0) {
        ret = NETLIB_ERROR;
    }
#endif

    return ret;
}

int netlib_listen(const char* server_ip, uint16_t port, const NewConnectionCallback& callback)
{
    BaseSocketPtr socket = std::make_shared<BaseSocket>();
    if (!socket) {
        return NETLIB_ERROR;
    }
    int ret = socket->Listen(server_ip, port, callback);
    return ret;
}

net_handle_t netlib_connect(const char* server_ip, uint16_t port, const MessageCallback& callback)
{
    BaseSocketPtr socket = std::make_shared<BaseSocket>();
    if (!socket) {
        return NETLIB_INVALID_HANDLE;
    }
    net_handle_t handle = socket->Connect(server_ip, port, callback);
    return handle;
}

int netlib_send(net_handle_t handle, void* buf, int len)
{
    BaseSocketPtr socket = FindBaseSocket(handle);
    if (!socket) {
        return NETLIB_ERROR;
    }
    int ret = socket->Send(buf, len);
    return ret;
}

int netlib_recv(net_handle_t handle, void* buf, int len)
{
    BaseSocketPtr socket = FindBaseSocket(handle);
    if (!socket) {
        return NETLIB_ERROR;
    }
    int ret = socket->Recv(buf, len);
    return ret;
}

int netlib_close(net_handle_t handle)
{
    BaseSocketPtr socket = FindBaseSocket(handle);
    if (!socket) {
        return NETLIB_ERROR;
    }
    int ret = socket->Close();
    return ret;
}

int netlib_option(net_handle_t handle, int opt, void* optval)
{
    BaseSocketPtr socket = FindBaseSocket(handle);
    if (!socket) {
        return NETLIB_ERROR;
    }

    if ((opt >= NETLIB_OPT_GET_REMOTE_IP) && !optval) {
        return NETLIB_ERROR;
    }

    switch (opt)
    {
    case NETLIB_OPT_GET_REMOTE_IP:
        *(std::string*)optval = socket->GetRemoteIP();
        break;
    case NETLIB_OPT_GET_REMOTE_PORT:
        *(uint16_t*)optval = socket->GetRemotePort();
        break;
    case NETLIB_OPT_GET_LOCAL_IP:
        *(std::string*)optval = socket->GetLocalIP();
        break;
    case NETLIB_OPT_GET_LOCAL_PORT:
        *(uint16_t*)optval = socket->GetLocalPort();
        break;
    case NETLIB_OPT_SET_SEND_BUF_SIZE:
        socket->SetSendBufSize(*(uint32_t*)optval);
        break;
    case NETLIB_OPT_SET_RECV_BUF_SIZE:
        socket->SetRecvBufSize(*(uint32_t*)optval);
        break;
    }

    return NETLIB_OK;
}

int netlib_register_message_callback(net_handle_t handle, const MessageCallback& cb)
{
    BaseSocketPtr socket = FindBaseSocket(handle);
    if (!socket) {
        return NETLIB_ERROR;
    }

    socket->SetMessageCallback(cb);

    return NETLIB_OK;
}

int netlib_register_timer(const TimerCallback& callback, uint64_t interval)
{
    EventDispatch::Instance()->AddTimer(callback, interval);
    return NETLIB_OK;
}

int netlib_delete_timer(const TimerCallback& callback)
{
    EventDispatch::Instance()->RemoveTimer(callback);
    return NETLIB_OK;
}

int netlib_add_loop(const LoopCallback& callback)
{
    EventDispatch::Instance()->AddLoop(callback);
    return NETLIB_OK;
}

void netlib_eventloop(uint32_t wait_timeout)
{
    EventDispatch::Instance()->StartDispatch(wait_timeout);
}

void netlib_start_event()
{
    if (!netlib_is_running())
    {
        netlib_init();
        EventDispatch::Instance()->StartDispatchInThread();
    }
}

void netlib_stop_event()
{
    EventDispatch::Instance()->StopDispatch();
}

bool netlib_is_running()
{
    return EventDispatch::Instance()->IsRunning();
}


NAMESPACE_END(net)
NAMESPACE_END(z)