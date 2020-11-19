/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: MsgServer与DbProxyServer之间的连接
 */

#include "stdafx.h"
#include "utils.h"
#include "im_pdu_base.h"
#include "thread_pool.h"
#include "im.base.pb.h"
#include "im.other.pb.h"
#include "proxy_conn.h"
#include "proxy_task.h"
#include "handler_map.h"

static ConnMap g_proxy_conn_map;
static UserMap g_uuid_conn_map;
static HandlerMapManager* g_handler_map_manager;

static ThreadPool g_thread_pool;

uint32_t ProxyConn::uuid_alloctor_ = 0;
std::mutex ProxyConn::mutex_;
std::list<ResponsePduPtr> ProxyConn::response_pdu_list_;

void proxy_timer_callback(uint8_t msg, uint32_t handle)
{
    uint64_t cur_time = z::utils::GetTickCount();
    auto it = g_proxy_conn_map.begin();
    for (; it != g_proxy_conn_map.end(); ) {
        auto it_old = it;
        ++it;
        ProxyConnPtr conn = std::dynamic_pointer_cast<ProxyConn>(it_old->second);
        conn->OnTimer(cur_time);
    }
}

void proxy_loop_callback(uint8_t msg, uint32_t handle)
{
    ProxyConn::SendResponsePduList();
}

// 用于优雅地关闭连接
#ifndef _WIN32
void exit_callback(uint8_t msg, uint32_t handle) {
    LOG_WARN("exit_callback...");
    exit(0);
}
static void sig_handler(int sig_no) {
    if (SIGTERM == sig_no) {
        LOG_WARN("receive SIGTERM, prepare for exit");
        z::net::netlib_register_timer(
            std::bind(&exit_callback, std::placeholders::_1,
                std::placeholders::_2), 4000
        );
    }
}
#endif

int init_proxy_conn(uint32_t thread_num)
{
    g_handler_map_manager = HandlerMapManager::GetInstance();
    g_thread_pool.Init(thread_num);

    z::net::netlib_add_loop(
        std::bind(&proxy_loop_callback, std::placeholders::_1, std::placeholders::_2));

#ifndef _WIN32
    signal(SIGTERM, sig_handler);
#endif
    return z::net::netlib_register_timer(
        std::bind(&proxy_timer_callback, std::placeholders::_1, 
            std::placeholders::_2), 1000
    );
}

ProxyConnPtr get_proxy_conn_by_uuid(uint32_t uuid)
{
    auto it = g_uuid_conn_map.find(uuid);
    if (it != g_uuid_conn_map.end()) {
        return std::dynamic_pointer_cast<ProxyConn>(it->second);
    }
    return nullptr;
}

ProxyConn::ProxyConn()
{
    
}

// 防止出现std::bad_weak_ptr, 必须等ProxyConn()构造完再更新g_uuid_conn_map
void ProxyConn::SetUuid()
{
    uuid_ = ++ProxyConn::uuid_alloctor_;
    if (0 == uuid_) {
        uuid_ = ++ProxyConn::uuid_alloctor_;
    }
    g_uuid_conn_map.insert(std::make_pair(uuid_, shared_from_this()));
}

ProxyConn::~ProxyConn()
{
}

void ProxyConn::Close()
{
    if (handle_ != NETLIB_INVALID_HANDLE) {
        z::net::netlib_close(handle_);
        g_proxy_conn_map.erase(handle_);
        g_uuid_conn_map.erase(uuid_);
    }
}

void ProxyConn::OnConnect(net_handle_t handle)
{
    handle_ = handle;
    g_proxy_conn_map.insert(std::make_pair(handle, shared_from_this()));
    
    z::net::netlib_register_message_callback(
        handle,
        std::bind(&ImConnCallback, std::placeholders::_1, std::placeholders::_2,
        (void*)&g_proxy_conn_map));
    
    z::net::netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void*)&peerIp_);
    z::net::netlib_option(handle, NETLIB_OPT_GET_REMOTE_PORT, (void*)&peerPort_);

    LOG_INFO("connect from %s:%d, handle:%d", peerIp_.c_str(), peerPort_, handle);
}

// 由于数据包是在另一个线程处理的，所以不能在主线程delete数据包，所以需要Override这个方法
void ProxyConn::OnRead()
{
    for (;;) {
        uint32_t freeBufLen = inBuf_.GetAllocSize() - inBuf_.GetWriteOffset();
        if (freeBufLen < READ_BUF_SIZE) {
            inBuf_.Extend(READ_BUF_SIZE);
        }
        int ret = z::net::netlib_recv(handle_, inBuf_.GetBuffer() + inBuf_.GetWriteOffset(), READ_BUF_SIZE);
        if (ret <= 0) {
            break;
        }
        recvBytes_ += ret;
        inBuf_.IncWriteOffset(ret);
        lastRecvTick_ = z::utils::GetTickCount();
    }
    uint32_t pduLen = 0;
    try {
        while (ImPdu::IsPduAvailable(inBuf_.GetBuffer(), inBuf_.GetWriteOffset(), pduLen)) {
            HandlePduBuf(inBuf_.GetBuffer(), pduLen);
            inBuf_.Read(NULL, pduLen);
        }
    } catch (PduException& ex) {
        LOG_WARN("!!!catch exception, err_code=%u, err_msg=%s, close the connection",
            ex.GetErrorCode(), ex.GetErrorMsg());
        OnClose();
    }
}

void ProxyConn::OnClose()
{
    LOG_WARN("ProxyConn::OnClose()");
    Close();
}

void ProxyConn::OnTimer(uint64_t curr_tick)
{
    if (curr_tick > lastSendTick_ + SERVER_HEARTBEAT_INTERVAL) {
        ImPdu pdu;
        im::other::HeartBeat msg;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(im::base::ServiceID::SID_OTHER);
        pdu.SetCommandId(im::base::OtherCmdID::CID_OTHER_HEARTBEAT);
        SendPdu(&pdu);
        //LOG_INFO("ProxyConn send CID_OTHER_HEARTBEAT");
    }
    if (curr_tick > lastRecvTick_ + SERVER_TIMEOUT) {
        LOG_WARN("proxy conn timeout %s:%d", peerIp_.c_str(), peerPort_);
        Close();
    }
}

void ProxyConn::HandlePduBuf(uchar_t* pdu_buf, uint32_t pdu_len)
{
    auto pdu = ImPdu::ReadPdu(pdu_buf, pdu_len);
    auto commandId = pdu->GetCommandId();
    if (im::base::OtherCmdID::CID_OTHER_HEARTBEAT == commandId) {
        //LOG_INFO("ProxyConn recv CID_OTHER_HEARTBEAT");
        return;
    }
    pdu_handler_t handler = g_handler_map_manager->GetHandler(commandId);
    if (handler) {
        std::shared_ptr<Task> task = std::make_shared<ProxyTask>(uuid_, handler, pdu);
        g_thread_pool.AddTask(task);
    } else {
        LOG_WARN("no handler for packet type: %d", commandId);
    }
}

void ProxyConn::AddResponsePdu(uint32_t conn_uuid, ImPduPtr pdu)
{
    ResponsePduPtr rspPdu = std::make_shared<ResponsePdu>();
    rspPdu->connUuid = conn_uuid;
    rspPdu->pdu = pdu;

    std::unique_lock<std::mutex> lock(mutex_);
    response_pdu_list_.push_back(rspPdu);
}

void ProxyConn::SendResponsePduList()
{
    std::unique_lock<std::mutex> lock(mutex_);

    while (!response_pdu_list_.empty()) {
        auto rspPdu = response_pdu_list_.front();
        auto conn = get_proxy_conn_by_uuid(rspPdu->connUuid);
        if (conn) {
            if (rspPdu->pdu) {
                conn->SendPdu(rspPdu->pdu);
            } else {
                LOG_ERROR("parse pdu error, close connection uuid: %d", rspPdu->connUuid);
                conn->Close();
            }
        }
        response_pdu_list_.pop_front();
    }
}
