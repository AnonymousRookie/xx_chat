/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: MsgServer与RouteServer之间的连接
 */

#include "stdafx.h"
#include <list>
#include "utils.h"
#include "im.base.pb.h"
#include "im.server.pb.h"
#include "im.other.pb.h"
#include "im_pdu_base.h"
#include "defines.h"
#include "route_server_conn.h"
#include "im_user.h"

static ConnMap g_route_server_conn_map;

static ServerListType* g_route_server_list = nullptr;
static uint32_t g_route_server_count = 0;
static RouteServConnPtr g_master_rs_conn = nullptr;


void route_server_conn_timer_callback(uint8_t msg, uint32_t handle)
{
	ConnMap::iterator it_old;
	RouteServConn* pConn = NULL;
    uint64_t cur_time = z::utils::GetTickCount();

	for (ConnMap::iterator it = g_route_server_conn_map.begin(); it != g_route_server_conn_map.end(); ) {
		it_old = it;
		it++;
		auto pConn = std::dynamic_pointer_cast<RouteServConn>(it_old->second);
		pConn->OnTimer(cur_time);
	}
    // reconnect route_server in 4s, 8s, 16s, 32s, 4s, 8s, ...
    serv_check_reconnect<RouteServConn>(g_route_server_list);
}

void init_route_serv_conn(ServerListType* server_list)
{
	g_route_server_list = server_list;
	serv_init<RouteServConn>(g_route_server_list);
	z::net::netlib_register_timer(
        std::bind(&route_server_conn_timer_callback, std::placeholders::_1, 
            std::placeholders::_2), 1000
    );
}

bool is_route_server_available()
{
    Z_CHECK_RETURN(g_route_server_list, false);
	for (uint32_t i = 0; i < g_route_server_list->size(); ++i) {
        auto conn = std::dynamic_pointer_cast<RouteServConn>(g_route_server_list->at(i)->serv_conn);
		if (conn && conn->IsOpen()) {
			return true;
		}
	}
	return false;
}

void send_to_all_route_server(std::shared_ptr<ImPdu> pPdu)
{
    Z_CHECK(g_route_server_list);
    for (uint32_t i = 0; i < g_route_server_list->size(); ++i) {
        auto conn = std::dynamic_pointer_cast<RouteServConn>(g_route_server_list->at(i)->serv_conn);
		if (conn && conn->IsOpen()) {
            conn->SendPdu(pPdu);
		}
	}
}

void update_master_route_serv_conn()
{
    Z_CHECK(g_route_server_list);

	uint64_t oldest_connect_time = (uint64_t)-1;
    RouteServConnPtr pOldestConn = nullptr;
    RouteServConnPtr pConn = nullptr;

    for (uint32_t i = 0; i < g_route_server_list->size(); ++i) {
		pConn = std::dynamic_pointer_cast<RouteServConn>(g_route_server_list->at(i)->serv_conn);
		if (pConn && pConn->IsOpen() && (pConn->GetConnectTime() < oldest_connect_time) ){
			pOldestConn = pConn;
			oldest_connect_time = pConn->GetConnectTime();
		}
	}

	g_master_rs_conn =  pOldestConn;

	if (g_master_rs_conn) {
        // ...
    }
}


RouteServConn::RouteServConn()
{
	opened_ = false;
	serverIndex_ = 0;
}

RouteServConn::~RouteServConn()
{

}

void RouteServConn::Connect(const char* serverIp, uint16_t serverPort, uint32_t serverIndex)
{
    LOG_INFO("Connecting to route_server %s:%d", serverIp, serverPort);

	serverIndex_ = serverIndex;
    handle_ = z::net::netlib_connect(serverIp, serverPort, 
        std::bind(&ImConnCallback, std::placeholders::_1, std::placeholders::_2,
        (void*)&g_route_server_conn_map));

	if (handle_ != NETLIB_INVALID_HANDLE) {
		g_route_server_conn_map.insert(std::make_pair(handle_, shared_from_this()));
	}
}


void RouteServConn::Close()
{
	serv_reset<RouteServConn>(g_route_server_list, serverIndex_);

	opened_ = false;
	if (handle_ != NETLIB_INVALID_HANDLE) {
		z::net::netlib_close(handle_);
		g_route_server_conn_map.erase(handle_);
	}

	if (g_master_rs_conn == shared_from_this()) {
		update_master_route_serv_conn();
	}
}

void RouteServConn::OnClose()
{
    LOG_WARN("RouteServConn OnClose from route_server handle=%d", handle_);
    Close();
}

void RouteServConn::OnConfirm()
{
    LOG_INFO("connect to route server success.");
	opened_ = true;
    connectTime_ = z::utils::GetTickCount();
	g_route_server_list->at(serverIndex_)->reconnect_cnt = MIN_RECONNECT_CNT / 2;

	if (g_master_rs_conn == NULL) {
		update_master_route_serv_conn();
	}

	std::list<user_stat_t> online_user_list;
    ImUserManager::GetInstance()->GetOnlineUserInfo(&online_user_list);

    im::server::OnlineUserInfo msg;
    for (auto it = online_user_list.begin(); it != online_user_list.end(); it++) {
        user_stat_t user_stat = *it;
        im::base::ServerUserStat* server_user_stat = msg.add_user_stat_list();
        server_user_stat->set_user_id(user_stat.user_id);
        server_user_stat->set_status((im::base::UserStatType)user_stat.status);
    }
    ImPdu pdu;
    pdu.SetPBMsg(&msg);
    pdu.SetServiceId(im::base::ServiceID::SID_OTHER);
    pdu.SetCommandId(im::base::OtherCmdID::CID_OTHER_ONLINE_USER_INFO);
	SendPdu(&pdu);
}


void RouteServConn::OnTimer(uint64_t currTick)
{
	if (currTick > lastSendTick_ + SERVER_HEARTBEAT_INTERVAL) {
        im::other::HeartBeat msg;
        ImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(im::base::ServiceID::SID_OTHER);
        pdu.SetCommandId(im::base::OtherCmdID::CID_OTHER_HEARTBEAT);
        SendPdu(&pdu);
        //LOG_INFO("RouteServConn send CID_OTHER_HEARTBEAT");
	}

	if (currTick > lastRecvTick_ + SERVER_TIMEOUT) {
        LOG_WARN("conn to route server timeout!");
		Close();
	}
}

void RouteServConn::HandlePdu(std::shared_ptr<ImPdu> pdu)
{
	switch (pdu->GetCommandId()) {
        case im::base::OtherCmdID::CID_OTHER_HEARTBEAT:
            //LOG_INFO("RouteServConn recv CID_OTHER_HEARTBEAT");
            break;
        default:
            LOG_WARN("RouteServConn unknown cmd id=%d ", pdu->GetCommandId());
            break;
	}
}
