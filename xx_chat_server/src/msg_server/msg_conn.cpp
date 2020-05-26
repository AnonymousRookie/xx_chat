/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: Client与MsgServer之间的连接
 */


#include "stdafx.h"
#include "im.base.pb.h"
#include "im.login.pb.h"
#include "im.server.pb.h"
#include "im.buddy.pb.h"
#include "utils.h"
#include "msg_conn.h"
#include "im_pdu_base.h"
#include "db_server_conn.h"
#include "login_server_conn.h"
#include "route_server_conn.h"
#include "im_user.h"
#include "attach_data.h"

#define TIMEOUT_WATI_LOGIN_RESPONSE 15000  // 15s

static ConnMap g_msg_conn_map;
static UserMap g_msg_conn_user_map;

void msg_conn_timer_callback(uint8_t msg, uint32_t handle)
{
    uint64_t cur_time = z::utils::GetTickCount();
    for (auto it = g_msg_conn_map.begin(); it != g_msg_conn_map.end();) {
        auto it_old = it;
        it++;
        auto conn = std::dynamic_pointer_cast<MsgConn>(it_old->second);
        conn->OnTimer(cur_time);
    }
}

void init_msg_conn()
{
    z::net::netlib_register_timer(
        std::bind(&msg_conn_timer_callback, std::placeholders::_1, 
            std::placeholders::_2), 1000
    );
}

MsgConn::MsgConn() 
    : userId_(0)
    , open_(false)
    , onlineStatus_(im::base::UserStatType::USER_STATUS_OFFLINE)
{
}

MsgConn::~MsgConn()
{
}

void MsgConn::Close()
{
    LOG_INFO("close client, handle=%d, user_id=%u", handle_, GetUserId());
    if (handle_ != NETLIB_INVALID_HANDLE) {
        z::net::netlib_close(handle_);
        g_msg_conn_map.erase(handle_);
    }
}

void MsgConn::OnClose()
{
    LOG_WARN("client closed!");
    Close();
}

void MsgConn::OnConnect(net_handle_t handle)
{
    handle_ = handle;
    loginTime_ = z::utils::GetTickCount();

    g_msg_conn_map.insert(std::make_pair(handle, shared_from_this()));

    z::net::netlib_register_message_callback(
        handle,
        std::bind(&ImConnCallback, std::placeholders::_1, std::placeholders::_2,
        (void*)&g_msg_conn_map));

    z::net::netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void*)&peerIp_);
    z::net::netlib_option(handle, NETLIB_OPT_GET_REMOTE_PORT, (void*)&peerPort_);
}

void MsgConn::OnTimer(uint64_t currTick)
{
    if (currTick > lastRecvTick_ + CLIENT_TIMEOUT) {
        LOG_WARN("client timeout, handle=%d, uid=%u", handle_, GetUserId());
        Close();
        return;
    }
    if (!IsOpen()) {
        if (currTick > loginTime_ + TIMEOUT_WATI_LOGIN_RESPONSE) {
            LOG_WARN("login timeout, handle=%d, uid=%u", handle_, GetUserId());
            Close();
            return;
        }
    }
}

void MsgConn::HandlePdu(std::shared_ptr<ImPdu> pdu)
{
    switch (pdu->GetCommandId())
    {
    case im::base::OtherCmdID::CID_OTHER_HEARTBEAT:
        //LOG_INFO("MsgConn recv and send CID_OTHER_HEARTBEAT");
        SendPdu(pdu);
        break;
    case im::base::LoginCmdID::CID_LOGIN_REQ_USERLOGIN:
        LOG_INFO("MsgConn recv CID_LOGIN_REQ_USERLOGIN");
        HandleLoginRequest(pdu);
        break;
    case im::base::BuddyListCmdID::CID_BUDDY_LIST_ALL_USER_REQ:
        HandleBuddyListAllUserRequest(pdu);
        break;
    default:
        LOG_WARN("MsgConn recv 未知消息: %d", pdu->GetCommandId());
        break;
    }
}

void MsgConn::HandleLoginRequest(std::shared_ptr<ImPdu> pdu)
{
    uint32_t result = 0;
    std::string resultStr("");
    auto dbConn = get_db_serv_conn_for_login();
    if (!dbConn) {
        result = im::base::REFUSE_REASON_NO_DB_SERVER;
        resultStr = "db_proxy_server异常";
    }
    else if (!is_login_server_available()) {
        result = im::base::REFUSE_REASON_NO_LOGIN_SERVER;
        resultStr = "login_server异常";
    }
    else if (!is_route_server_available()) {
        result = im::base::REFUSE_REASON_NO_ROUTE_SERVER;
        resultStr = "route_server异常";
    }

    if (result != 0) {
        im::login::LoginRes resMsg;
        resMsg.set_server_time(time(NULL));
        resMsg.set_result_code(static_cast<im::base::ResultType>(result));
        resMsg.set_result_string(resultStr);

        ImPdu pdu;
        pdu.SetPBMsg(&resMsg);
        pdu.SetServiceId(im::base::SID_LOGIN);
        pdu.SetCommandId(im::base::CID_LOGIN_RES_USERLOGIN);
        pdu.SetSeqNum(pdu.GetSeqNum());
        SendPdu(&pdu);
        Close();
        return;
    }

    // 解析登录请求
    im::login::LoginReq loginReq;
    loginReq.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength());

    loginName_ = loginReq.username();
    std::string passwd = loginReq.password();
    onlineStatus_ = loginReq.online_status();

    LOG_INFO("HandleLoginRequest, username=%s, onlinestatus=%u", loginName_.c_str(), onlineStatus_);

    ImUser* imUser = ImUserManager::GetInstance()->GetImUserByName(loginName_);
    if (!imUser) {
        imUser = new ImUser(loginName_);
        ImUserManager::GetInstance()->AddImUserByName(loginName_, imUser);
    }
    imUser->AddUnValidateMsgConn(this);

    AttachData attachData(attachDataType_Handle, handle_, 0);

    // 向db_proxy_server发送登录验证请求
    im::server::LoginValidateReq loginValidateReq;
    loginValidateReq.set_user_name(loginName_);
    loginValidateReq.set_password(passwd);
    loginValidateReq.set_attach_data(attachData.GetBuffer(), attachData.GetLength());

    ImPdu loginValidateReqPdu;
    loginValidateReqPdu.SetPBMsg(&loginValidateReq);
    loginValidateReqPdu.SetServiceId(im::base::ServiceID::SID_OTHER);
    loginValidateReqPdu.SetCommandId(im::base::OtherCmdID::CID_OTHER_LOGIN_VALIDATE_REQ);
    loginValidateReqPdu.SetSeqNum(pdu->GetSeqNum());
    dbConn->SendPdu(&loginValidateReqPdu);
}

void MsgConn::HandleBuddyListAllUserRequest(std::shared_ptr<ImPdu> pdu)
{
    im::buddy::AllUserReq msg;
    msg.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength());

    DBServConnPtr dbconn = get_db_serv_conn();
    if (dbconn) {
        AttachData attachData(attachDataType_Handle, handle_, 0);
        msg.set_user_id(GetUserId());
        msg.set_attach_data(attachData.GetBuffer(), attachData.GetLength());
        pdu->SetPBMsg(&msg);
        dbconn->SendPdu(pdu);
    }
}