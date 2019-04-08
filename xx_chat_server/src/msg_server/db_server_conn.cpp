/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: MsgServer与DbProxyServer之间的连接
 */

#include "stdafx.h"
#include <memory>
#include "im.base.pb.h"
#include "im.other.pb.h"
#include "im.server.pb.h"
#include "im.login.pb.h"
#include "im_pdu_base.h"
#include "db_server_conn.h"
#include "im_user.h"
#include "msg_conn.h"
#include "attach_data.h"

static ConnMap g_db_server_conn_map;
static ServerListType* g_db_server_list = NULL;
static uint32_t g_db_server_count = 0;        // DBServer的总连接数
static uint32_t g_db_server_login_count = 0;  // 进行登录处理的DBServer的总连接数


static void db_server_conn_timer_callback(uint8_t msg, uint32_t handle)
{
    uint64_t cur_time = z::utils::GetTickCount();
    for (auto it = g_db_server_conn_map.begin(); it != g_db_server_conn_map.end();) {
        auto it_old = it;
        ++it;
        auto conn = std::dynamic_pointer_cast<DBServConn>(it_old->second);
        if (conn->IsOpen()) {
            conn->OnTimer(cur_time);
        }
    }
    // reconnect db_proxy_server in 4s, 8s, 16s, 32s, 4s, 8s, ...
    serv_check_reconnect<DBServConn>(g_db_server_list);
}

void init_db_serv_conn(ServerListType* server_list)
{
    Z_CHECK(server_list);
    g_db_server_list = server_list;
    g_db_server_count = server_list->size();

    uint32_t total_db_instance = server_list->size();
    g_db_server_login_count = total_db_instance;

    LOG_INFO("DB server connection index for login business: [0, %u), for other business: [%u, %u) ",
        g_db_server_login_count, g_db_server_login_count, g_db_server_count);

    serv_init<DBServConn>(g_db_server_list);

    z::net::netlib_register_timer(
        std::bind(&db_server_conn_timer_callback, std::placeholders::_1, std::placeholders::_2),
        1000);
}

// get a random db server connection in the range [start_pos, stop_pos)
DBServConnPtr get_db_server_conn_in_range(uint32_t start_pos, uint32_t stop_pos)
{
    uint32_t i = 0;
    DBServConnPtr conn = nullptr;
    // determine if there is a valid DB server connection
    for (i = start_pos; i < stop_pos; ++i) {
        conn = std::dynamic_pointer_cast<DBServConn>(g_db_server_list->at(i)->serv_conn);
        if (conn && conn->IsOpen()) {
            break;
        }
    }
    // no valid DB server connection
    if (i == stop_pos) {
        return NULL;
    }
    // return a random valid DB server connection
    while (true) {
        int random = rand() % (stop_pos - start_pos) + start_pos;
        conn = std::dynamic_pointer_cast<DBServConn>(g_db_server_list->at(i)->serv_conn);
        if (conn && conn->IsOpen()) {
            break;
        }
    }
    return conn;
}

DBServConnPtr get_db_serv_conn_for_login()
{
    // 先获取login业务的实例, 没有就去获取其他业务流程的实例
    DBServConnPtr conn = get_db_server_conn_in_range(0, g_db_server_login_count);
    if (!conn) {
        conn = get_db_server_conn_in_range(g_db_server_login_count, g_db_server_count);
    }
    return conn;
}

DBServConnPtr get_db_serv_conn()
{
    // 先获取其他业务流程的实例, 没有就去获取login业务的实例
    DBServConnPtr conn = get_db_server_conn_in_range(g_db_server_login_count, g_db_server_count);
    if (!conn) {
        conn = get_db_server_conn_in_range(0, g_db_server_login_count);
    }
    return conn;
}

DBServConn::DBServConn()
{
    opened_ = false;
}

DBServConn::~DBServConn()
{

}

void DBServConn::Connect(const char* serverIp, uint16_t serverPort, uint32_t servIndex)
{
    LOG_INFO("connecting to db_proxy_server %s:%d", serverIp, serverPort);
    serverIndex_ = servIndex;
    handle_ = z::net::netlib_connect(serverIp, serverPort,
                                     std::bind(&ImConnCallback, std::placeholders::_1, std::placeholders::_2,
                                     (void*)&g_db_server_conn_map));
    if (handle_ != NETLIB_INVALID_HANDLE) {
        g_db_server_conn_map.insert(std::make_pair(handle_, shared_from_this()));
    }
}

void DBServConn::Close()
{
    // reset server information for the next connect
    serv_reset<DBServConn>(g_db_server_list, serverIndex_);
    if (handle_ != NETLIB_INVALID_HANDLE) {
        z::net::netlib_close(handle_);
        g_db_server_conn_map.erase(handle_);
    }
}

void DBServConn::OnConfirm()
{
    LOG_INFO("connect to db server success");
    opened_ = true;
}

void DBServConn::OnClose()
{
    LOG_WARN("DBServConn OnClose from db_proxy_server handle=%d", handle_);
    Close();
}

void DBServConn::OnTimer(uint64_t currTick)
{
    if (currTick > lastSendTick_ + SERVER_HEARTBEAT_INTERVAL) {
        im::other::HeartBeat msg;
        ImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(im::base::ServiceID::SID_OTHER);
        pdu.SetCommandId(im::base::OtherCmdID::CID_OTHER_HEARTBEAT);
        SendPdu(&pdu);
        //LOG_INFO("DBServConn Send CID_OTHER_HEARTBEAT");
    }
    if (currTick > lastSendTick_ + SERVER_TIMEOUT) {
        LOG_WARN("conn to db server timeout");
        Close();
    }
}

void DBServConn::HandlePdu(ImPdu* pdu)
{
    switch (pdu->GetCommandId()) {
    case im::base::OtherCmdID::CID_OTHER_HEARTBEAT:
        //LOG_INFO("DBServConn recv CID_OTHER_HEARTBEAT");
        break;
    case im::base::OtherCmdID::CID_OTHER_LOGIN_VALIDATE_RSP:
        HandleLoginValidResponse(pdu);
        break;
    default:
        LOG_WARN("DBServConn unknown cmd id=%d", pdu->GetCommandId());
        break;
    }
}

void DBServConn::HandleLoginValidResponse(ImPdu* pdu)
{
    im::server::LoginValidateRsp loginValidateRsp;
    loginValidateRsp.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength());

    const std::string& loginName = loginValidateRsp.user_name();
    uint32_t result = loginValidateRsp.result_code();
    const std::string& resultStr = loginValidateRsp.result_string();

    AttachData attachData((uchar_t*)loginValidateRsp.attach_data().c_str(), loginValidateRsp.attach_data().length());

    LOG_INFO("HandleLoginValidResponse, username: %s, resultstr: %s", loginName.c_str(), resultStr.c_str());

    ImUser* imUser = ImUserManager::GetInstance()->GetImUserByName(loginName);
    if (!imUser) {
        LOG_WARN("ImUser for username:%s not exist", loginName.c_str());
        return;
    }
    MsgConn* msgConn = imUser->GetUnValidateMsgConn(attachData.GetHandle());
    if (!msgConn || msgConn->IsOpen()) {
        LOG_WARN("no such conn is validated, username: %s", loginName.c_str());
        return;
    }
    if (result != 0) {
        im::login::LoginRes loginRes;
        loginRes.set_server_time(time(NULL));
        result = im::base::REFUSE_REASON_DB_VALIDATE_FAILED;
        loginRes.set_result_code(static_cast<im::base::ResultType>(result));
        loginRes.set_result_string(resultStr);

        ImPdu pdu;
        pdu.SetPBMsg(&loginRes);
        pdu.SetServiceId(im::base::ServiceID::SID_LOGIN);
        pdu.SetCommandId(im::base::LoginCmdID::CID_LOGIN_RES_USERLOGIN);

        msgConn->SendPdu(&pdu);
        msgConn->Close();
        return;
    }

    im::base::UserInfo userInfo = loginValidateRsp.user_info();
    uint32_t userId = userInfo.user_id();
    ImUser* user = ImUserManager::GetInstance()->GetImUserById(userId);
    if (!user) {
        user = imUser;
    } else {
        user->AddUnValidateMsgConn(msgConn);
        imUser->DelUnValidateMsgConn(msgConn);
        if (imUser->IsMsgConnEmpty()) {
            ImUserManager::GetInstance()->RemoveImUserByName(loginName);
        }
    }
    user->SetUserId(userId);
    user->SetValidate();
    ImUserManager::GetInstance()->AddImUserById(userId, user);
    msgConn->SetUserId(userId);
    msgConn->SetOpen();

    im::login::LoginRes loginRes;
    loginRes.set_server_time(time(NULL));
    loginRes.set_result_code(im::base::REFUSE_REASON_NONE);
    loginRes.set_result_string(resultStr);
    loginRes.set_online_status((im::base::UserStatType)msgConn->GetOnlineStatus());
    im::base::UserInfo* mutableUserInfo = loginRes.mutable_user_info();
    mutableUserInfo->set_user_id(userInfo.user_id());
    mutableUserInfo->set_user_name(userInfo.user_name());

    ImPdu loginResPdu;
    loginResPdu.SetPBMsg(&loginRes);
    loginResPdu.SetServiceId(im::base::ServiceID::SID_LOGIN);
    loginResPdu.SetCommandId(im::base::LoginCmdID::CID_LOGIN_RES_USERLOGIN);
    loginResPdu.SetSeqNum(loginResPdu.GetSeqNum());
    msgConn->SendPdu(&loginResPdu);
}
