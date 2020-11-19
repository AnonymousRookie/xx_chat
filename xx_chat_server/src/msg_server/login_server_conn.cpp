/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: MsgServer与LoginServer之间的连接
 */

#include "stdafx.h"
#include <memory>
#include <list>
#include "im.base.pb.h"
#include "im.server.pb.h"
#include "im.other.pb.h"
#include "im_pdu_base.h"
#include "utils.h"
#include "defines.h"
#include "login_server_conn.h"
#include "im_user.h"


static ConnMap g_login_server_conn_map;
static ServerListType* g_login_server_list = NULL;
static std::string g_msg_server_ip_addr;
static uint16_t g_msg_server_port;
static uint32_t g_max_conn_cnt;

void login_server_conn_timer_callback(uint8_t msg, uint32_t handle)
{
    uint64_t cur_time = z::utils::GetTickCount();
	for (ConnMap::iterator it = g_login_server_conn_map.begin(); it != g_login_server_conn_map.end(); ) {
		auto it_old = it;
		it++;
		auto pConn = std::dynamic_pointer_cast<LoginServConn>(it_old->second);
		pConn->OnTimer(cur_time);
	}
    // reconnect login_server in 4s, 8s, 16s, 32s, 4s, 8s, ...
    serv_check_reconnect<LoginServConn>(g_login_server_list);
}

void init_login_serv_conn(ServerListType* server_list,
                          const char* msg_server_ip_addr,
                          uint16_t msg_server_port,
                          uint32_t max_conn_cnt)
{
	g_login_server_list = server_list;

	serv_init<LoginServConn>(g_login_server_list);

	g_msg_server_ip_addr = msg_server_ip_addr;
	g_msg_server_port = msg_server_port;
	g_max_conn_cnt = max_conn_cnt;

	z::net::netlib_register_timer(
        std::bind(&login_server_conn_timer_callback, std::placeholders::_1, 
            std::placeholders::_2), 1000
    );
}

// if there is one LoginServer available, return true
bool is_login_server_available()
{
    Z_CHECK_RETURN(g_login_server_list, false);
	for (uint32_t i = 0; i < g_login_server_list->size(); i++) {
        auto conn = std::dynamic_pointer_cast<LoginServConn>(g_login_server_list->at(i)->serv_conn);
		if (conn && conn->IsOpen()) {
			return true;
		}
	}
	return false;
}

void send_to_all_login_server(std::shared_ptr<ImPdu> pPdu)
{
    Z_CHECK(g_login_server_list);
	for (uint32_t i = 0; i < g_login_server_list->size(); i++) {
        auto conn = std::dynamic_pointer_cast<LoginServConn>(g_login_server_list->at(i)->serv_conn);
		if (conn && conn->IsOpen()) {
            conn->SendPdu(pPdu);
		}
	}
}

LoginServConn::LoginServConn()
{
	opened_ = false;
}

LoginServConn::~LoginServConn()
{

}

void LoginServConn::Connect(const char* serverIp, uint16_t serverPort, uint32_t serverIndex)
{
    LOG_INFO("Connecting to login_server %s:%d", serverIp, serverPort);
	serverIndex_ = serverIndex;
	handle_ = z::net::netlib_connect(serverIp, serverPort, 
        std::bind(&ImConnCallback, std::placeholders::_1, std::placeholders::_2,
        (void*)&g_login_server_conn_map));

	if (handle_ != NETLIB_INVALID_HANDLE) {
		g_login_server_conn_map.insert(std::make_pair(handle_, shared_from_this()));
	}
}

void LoginServConn::Close()
{
	serv_reset<LoginServConn>(g_login_server_list, serverIndex_);
	if (handle_ != NETLIB_INVALID_HANDLE) {
        z::net::netlib_close(handle_);
		g_login_server_conn_map.erase(handle_);
	}
}

void LoginServConn::OnConfirm()
{
    LOG_INFO("connect to login server success.");
    Z_CHECK(g_login_server_list);

	opened_ = true;
	g_login_server_list->at(serverIndex_)->reconnect_cnt = MIN_RECONNECT_CNT / 2;

	uint32_t cur_conn_cnt = 0;
	uint32_t shop_user_cnt = 0;
    
    std::list<user_conn_t> user_conn_list;
    ImUserManager::GetInstance()->GetUserConnCnt(&user_conn_list, &cur_conn_cnt);

	char hostname[256] = {0};
	gethostname(hostname, 256);
    im::server::MsgServInfo msg;
    msg.set_ip(g_msg_server_ip_addr);
    msg.set_port(g_msg_server_port);
    msg.set_max_conn_cnt(g_max_conn_cnt);
    msg.set_cur_conn_cnt(cur_conn_cnt);
    msg.set_host_name(hostname);

    ImPdu pdu;
    pdu.SetPBMsg(&msg);
    pdu.SetServiceId(im::base::ServiceID::SID_OTHER);
    pdu.SetCommandId(im::base::OtherCmdID::CID_OTHER_MSG_SERV_INFO);
	SendPdu(&pdu);
}

void LoginServConn::OnClose()
{
    LOG_WARN("LoginServConn onclose from login_server, from handle=%d", handle_);
	Close();
}

void LoginServConn::OnTimer(uint64_t currTick)
{
	if (currTick > lastSendTick_ + SERVER_HEARTBEAT_INTERVAL) {
        im::other::HeartBeat msg;
        ImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(im::base::ServiceID::SID_OTHER);
        pdu.SetCommandId(im::base::OtherCmdID::CID_OTHER_HEARTBEAT);
		SendPdu(&pdu);
        //LOG_INFO("LoginServConn Send CID_OTHER_HEARTBEAT");
	}
	if (currTick > lastRecvTick_ + SERVER_TIMEOUT) {
        LOG_WARN("conn to login server timeout ");
		Close();
	}
}


void LoginServConn::HandlePdu(std::shared_ptr<ImPdu> pdu)
{
    switch (pdu->GetCommandId()) {
    case im::base::OtherCmdID::CID_OTHER_HEARTBEAT:
        //LOG_INFO("LoginServConn recv CID_OTHER_HEARTBEAT");
        break;
    default:
        LOG_WARN("LoginServConn unknown cmd id=%d", pdu->GetCommandId());
        break;
    }
}
