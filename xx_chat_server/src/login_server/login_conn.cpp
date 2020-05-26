/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: LoginServer与MsgServer之间的连接
 */

#include "stdafx.h"
#include <map>
#include "im.base.pb.h"
#include "im.server.pb.h"
#include "im.other.pb.h"
#include "im.login.pb.h"
#include "login_conn.h"
#include "utils.h"
#include "im_pdu_base.h"
#include "defines.h"

static ConnMap g_client_conn_map;
static ConnMap g_msg_serv_conn_map;
static uint32_t g_total_online_user_cnt = 0;
std::map<uint32_t, msg_serv_info_t*> g_msg_serv_info;

void login_conn_timer_callback(uint8_t msg, uint32_t handle)
{
    uint64_t cur_time = z::utils::GetTickCount();
    for (auto it = g_client_conn_map.begin(); it != g_client_conn_map.end();) {
        auto it_old = it;
        ++it;
        auto pConn = std::dynamic_pointer_cast<LoginConn>(it_old->second);
        pConn->OnTimer(cur_time);
    }
    for (auto it = g_msg_serv_conn_map.begin(); it != g_msg_serv_conn_map.end();) {
        auto it_old = it;
        it++;
        auto pConn = std::dynamic_pointer_cast<LoginConn>(it_old->second);
        pConn->OnTimer(cur_time);
    }
}

void init_login_conn()
{
    z::net::netlib_register_timer(
        std::bind(&login_conn_timer_callback, std::placeholders::_1, 
            std::placeholders::_2), 1000
    );
}

LoginConn::LoginConn()
{
}

LoginConn::~LoginConn()
{
}

void LoginConn::Close()
{
    if (NETLIB_INVALID_HANDLE == handle_) {
        return;
    }
    z::net::netlib_close(handle_);
    if (connType_ == LOGIN_CONN_TYPE_CLIENT) {
        g_client_conn_map.erase(handle_);
    } else {
        g_msg_serv_conn_map.erase(handle_);
        // remove all user count from this message server
        auto it = g_msg_serv_info.find(handle_);
        if (it != g_msg_serv_info.end()) {
            msg_serv_info_t* pMsgServInfo = it->second;
            g_total_online_user_cnt -= pMsgServInfo->cur_conn_cnt;
            delete pMsgServInfo;
            g_msg_serv_info.erase(it);
        }
    }
}

void LoginConn::OnConnect2(net_handle_t handle, int conn_type)
{
    handle_ = handle;
    connType_ = conn_type;
    ConnMap* conn_map = NULL;

    if (conn_type == LOGIN_CONN_TYPE_CLIENT) {
        conn_map = &g_client_conn_map;
    } else {
        conn_map = &g_msg_serv_conn_map;
        conn_map->insert(std::make_pair(handle, shared_from_this()));
    }

    z::net::netlib_register_message_callback(
        handle,
        std::bind(&ImConnCallback, std::placeholders::_1, std::placeholders::_2,
        (void*)conn_map)
    );
}

void LoginConn::OnClose()
{
    LOG_WARN("LoginConn onclose from msg_server handle=%d", handle_);
    Close();
}

void LoginConn::OnTimer(uint64_t curr_tick)
{
    if (connType_ == LOGIN_CONN_TYPE_CLIENT) {
        if (curr_tick > lastRecvTick_ + CLIENT_TIMEOUT) {
            Close();
        }
    } else {
        if (curr_tick > lastSendTick_ + SERVER_HEARTBEAT_INTERVAL) {
            im::other::HeartBeat msg;
            ImPdu pdu;
            pdu.SetPBMsg(&msg);
            pdu.SetServiceId(im::base::ServiceID::SID_OTHER);
            pdu.SetCommandId(im::base::OtherCmdID::CID_OTHER_HEARTBEAT);
            SendPdu(&pdu);
            //LOG_INFO("LoginConn Send CID_OTHER_HEARTBEAT");
        }
        if (curr_tick > lastRecvTick_ + SERVER_TIMEOUT) {
            LOG_WARN("connection to msg_server timeout!");
            Close();
        }
    }
}

void LoginConn::HandlePdu(std::shared_ptr<ImPdu> pPdu)
{
    switch(pPdu->GetCommandId()) {
        case im::base::OtherCmdID::CID_OTHER_HEARTBEAT:
            //LOG_INFO("LoginConn recv CID_OTHER_HEARTBEAT");
            break;
        case im::base::OtherCmdID::CID_OTHER_MSG_SERV_INFO:
            HandleMsgServInfo(pPdu);
            break;
        case im::base::OtherCmdID::CID_OTHER_USER_CNT_UPDATE:
            HandleUserCntUpdate(pPdu);
            break;
        case im::base::LoginCmdID::CID_LOGIN_REQ_MSGSERVER:
            HandleMsgServRequest(pPdu);
            break;
        default:
            LOG_WARN("wrong msg, cmd id = %d", pPdu->GetCommandId());
            break;
    }
}

void LoginConn::HandleMsgServInfo(std::shared_ptr<ImPdu> pPdu)
{
    msg_serv_info_t* pMsgServInfo = new msg_serv_info_t;
    im::server::MsgServInfo msg;

    msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength());

    pMsgServInfo->ip_addr = msg.ip();
    pMsgServInfo->port = msg.port();
    pMsgServInfo->max_conn_cnt = msg.max_conn_cnt();
    pMsgServInfo->cur_conn_cnt = msg.cur_conn_cnt();
    pMsgServInfo->hostname = msg.host_name();
    g_msg_serv_info.insert(std::make_pair(handle_, pMsgServInfo));

    g_total_online_user_cnt += pMsgServInfo->cur_conn_cnt;

    LOG_INFO("MsgServInfo, ip_addr=%s, port=%d, max_conn_cnt=%d, cur_conn_cnt=%d, hostname: %s",
        pMsgServInfo->ip_addr.c_str(), pMsgServInfo->port,pMsgServInfo->max_conn_cnt,
        pMsgServInfo->cur_conn_cnt, pMsgServInfo->hostname.c_str());
}


void LoginConn::HandleUserCntUpdate(std::shared_ptr<ImPdu> pPdu)
{
    auto it = g_msg_serv_info.find(handle_);
    if (it != g_msg_serv_info.end()) {
        msg_serv_info_t* pMsgServInfo = it->second;
        im::server::UserCntUpdate msg;
        msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength());

        uint32_t action = msg.user_action();
        if (action == USER_CNT_INC) {
            pMsgServInfo->cur_conn_cnt++;
            g_total_online_user_cnt++;
        } else {
            pMsgServInfo->cur_conn_cnt--;
            g_total_online_user_cnt--;
        }

        LOG_INFO("%s:%d, cur_cnt=%u, total_cnt=%u", pMsgServInfo->hostname.c_str(),
            pMsgServInfo->port, pMsgServInfo->cur_conn_cnt, g_total_online_user_cnt);
    }
}

void LoginConn::HandleMsgServRequest(std::shared_ptr<ImPdu> pPdu)
{
    im::login::MsgServReq reqMsg;
    reqMsg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength());

    // no message server available
    if (g_msg_serv_info.size() == 0) {
        im::login::MsgServRsp rspMsg;
        rspMsg.set_result_code(im::base::ResultType::REFUSE_REASON_NO_MSG_SERVER);
        ImPdu pdu;
        pdu.SetPBMsg(&rspMsg);
        pdu.SetServiceId(im::base::ServiceID::SID_LOGIN);
        pdu.SetCommandId(im::base::LoginCmdID::CID_LOGIN_RES_MSGSERVER);
        pdu.SetSeqNum(pPdu->GetSeqNum());
        SendPdu(&pdu);
        Close();
        return;
    }

    // return a message server with minimum concurrent connection count
    msg_serv_info_t* pMsgServInfo;
    uint32_t min_user_cnt = (uint32_t)-1;
    auto it_min_conn = g_msg_serv_info.end();

    for (auto it = g_msg_serv_info.begin() ; it != g_msg_serv_info.end(); ++it) {
        pMsgServInfo = it->second;
        if ( (pMsgServInfo->cur_conn_cnt < pMsgServInfo->max_conn_cnt) &&
             (pMsgServInfo->cur_conn_cnt < min_user_cnt)) {
            it_min_conn = it;
            min_user_cnt = pMsgServInfo->cur_conn_cnt;
        }
    }

    if (it_min_conn == g_msg_serv_info.end()) {
        LOG_WARN("all tcp msg_server are full!");
        im::login::MsgServRsp rspMsg;
        rspMsg.set_result_code(im::base::ResultType::REFUSE_REASON_MSG_SERVER_FULL);
        ImPdu pdu;
        pdu.SetPBMsg(&rspMsg);
        pdu.SetServiceId(im::base::ServiceID::SID_LOGIN);
        pdu.SetCommandId(im::base::LoginCmdID::CID_LOGIN_RES_MSGSERVER);
        pdu.SetSeqNum(pPdu->GetSeqNum());
        SendPdu(&pdu);
    }
    else {
        im::login::MsgServRsp rspMsg;
        rspMsg.set_result_code(im::base::ResultType::REFUSE_REASON_NONE);
        rspMsg.set_ip(it_min_conn->second->ip_addr);
        rspMsg.set_port(it_min_conn->second->port);
        ImPdu pdu;
        pdu.SetPBMsg(&rspMsg);
        pdu.SetServiceId(im::base::ServiceID::SID_LOGIN);
        pdu.SetCommandId(im::base::LoginCmdID::CID_LOGIN_RES_MSGSERVER);
        pdu.SetSeqNum(pPdu->GetSeqNum());
        SendPdu(&pdu);
    }

    Close();  // after send MsgServResponse, active close the connection
}
