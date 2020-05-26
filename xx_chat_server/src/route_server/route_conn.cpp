/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: MsgServer与RouteServer之间的连接
 */

#include "stdafx.h"
#include <unordered_map>
#include <set>
#include "im.message.pb.h"
#include "im.other.pb.h"
#include "im.server.pb.h"
#include "im_pdu_base.h"
#include "utils.h"
#include "net_lib.h"
#include "route_conn.h"
#include "user_info.h"


static ConnMap g_route_conn_map;
typedef std::unordered_map<uint32_t, UserInfoPtr> UserInfoMap_t;
static UserInfoMap_t g_user_map;

UserInfoPtr GetUserInfo(uint32_t userId)
{
    auto it = g_user_map.find(userId);
    if (it != g_user_map.end()) {
        return it->second;
    }
    return nullptr;
}

void route_serv_timer_callback(uint8_t msg, uint32_t handle)
{
    uint64_t curTime = z::utils::GetTickCount();
    ConnMap::iterator it = g_route_conn_map.begin();
    for (; it != g_route_conn_map.end(); ) {
        ConnMap::iterator itOld = it;
        ++it;
        RouteConnPtr conn = std::dynamic_pointer_cast<RouteConn>(itOld->second);
        conn->OnTimer(curTime);
    }
}

void init_routeconn_timer_callback()
{
    z::net::netlib_register_timer(
        std::bind(&route_serv_timer_callback, std::placeholders::_1, 
            std::placeholders::_2), 1000
    );
}

RouteConn::RouteConn()
{
    master_ = false;
}

RouteConn::~RouteConn()
{

}

void RouteConn::Close()
{
    if (handle_ != NETLIB_INVALID_HANDLE) {
        z::net::netlib_close(handle_);
        g_route_conn_map.erase(handle_);
    }
    // remove all user info from this MsgServer
    UserInfoMap_t::iterator itOld;
    for (auto it = g_user_map.begin(); it != g_user_map.end(); ) {
        itOld = it;
        ++it;
        auto user = itOld->second;
        user->RemoveRouteConn(std::dynamic_pointer_cast<RouteConn>(shared_from_this()));
        if (0 == user->GetRouteConnCount()) {
            g_user_map.erase(itOld);
        }
    }
}

void RouteConn::OnConnect(net_handle_t handle)
{
    handle_ = handle;
    g_route_conn_map.insert(std::make_pair(handle, shared_from_this()));

    z::net::netlib_register_message_callback(
        handle,
        std::bind(&ImConnCallback, std::placeholders::_1, std::placeholders::_2,
        (void*)&g_route_conn_map));
}

void RouteConn::OnClose()
{
    LOG_WARN("RouteConn onclose: handle=%d", handle_);
	Close();
}

void RouteConn::OnTimer(uint64_t currTick)
{
    if (currTick > lastSendTick_ + SERVER_HEARTBEAT_INTERVAL) {
        im::other::HeartBeat msg;
        ImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(im::base::ServiceID::SID_OTHER);
        pdu.SetCommandId(im::base::OtherCmdID::CID_OTHER_HEARTBEAT);
        SendPdu(&pdu);
        //LOG_INFO("RouteConn send CID_OTHER_HEARTBEAT");
    }

    if (currTick > lastRecvTick_ + SERVER_TIMEOUT) {
        LOG_WARN("recv msg from msg_server timeout!");
        Close();
    }
}

void RouteConn::HandlePdu(std::shared_ptr<ImPdu> pdu)
{
    switch(pdu->GetCommandId())
    {
        case im::base::OtherCmdID::CID_OTHER_HEARTBEAT:
            //LOG_INFO("RouteConn recv CID_OTHER_HEARTBEAT");
            break;
        case im::base::OtherCmdID::CID_OTHER_ONLINE_USER_INFO:
            HandleOnlineUserInfo(pdu);
            break;
        default:
            LOG_WARN("RouteConn::HandlePdu, wrong cmd id: %d ", pdu->GetCommandId());
            break;
    }
}

void RouteConn::HandleOnlineUserInfo(std::shared_ptr<ImPdu> pdu)
{
    im::server::OnlineUserInfo msg;
    msg.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength());
    uint32_t userCount = msg.user_stat_list_size();
    LOG_INFO("HandleOnlineUserInfo userCount = %u", userCount);
}

void RouteConn::SendPduToUser(uint32_t userId, std::shared_ptr<ImPdu> pdu, bool all)
{
    auto user = GetUserInfo(userId);
    if (!user) {
        return;
    }
    auto userSet = user->GetRouteConn();
    Z_CHECK(userSet);
    auto it = userSet->begin();
    for (; it != userSet->end(); ++it) {
        auto conn = *it;
        if (all || conn != std::dynamic_pointer_cast<RouteConn>(shared_from_this())) {
            conn->Send(pdu->GetBuffer(), pdu->GetLength());
        }
    }
}
