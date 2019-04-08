/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 处理用户登录请求
 */

#include "../stdafx.h"
#include "im.server.pb.h"
#include "login.h"
#include "inter_login.h"
#include "../proxy_conn.h"

InterLoginStrategy g_interLoginStrategy;


NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(business)

void DoLogin(ImPdu* pdu, uint32_t connUuid)
{
    ImPduPtr responsePdu = std::make_shared<ImPdu>();

    im::server::LoginValidateReq reqMsg;
    im::server::LoginValidateRsp rspMsg;

    if (reqMsg.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength())) {
        rspMsg.set_user_name(reqMsg.user_name());
        rspMsg.set_attach_data(reqMsg.attach_data());

        LOG_INFO("%s request login", reqMsg.user_name().c_str());

        im::base::UserInfo userInfo;
        if (g_interLoginStrategy.DoLogin(reqMsg.user_name(), reqMsg.password(), userInfo)) {
            im::base::UserInfo* pUserInfo = rspMsg.mutable_user_info();
            pUserInfo->set_user_id(userInfo.user_id());
            pUserInfo->set_user_name(userInfo.user_name());
            rspMsg.set_result_code(0);
            rspMsg.set_result_string("Login validate success!");
        }
        else {
            rspMsg.set_result_code(1);
            rspMsg.set_result_string("username or passwd error!");
        }
    }
    else {
        rspMsg.set_result_code(2);
        rspMsg.set_result_string("server error!");
    }
    responsePdu->SetPBMsg(&rspMsg);
    responsePdu->SetSeqNum(pdu->GetSeqNum());
    responsePdu->SetServiceId(im::base::ServiceID::SID_OTHER);
    responsePdu->SetCommandId(im::base::OtherCmdID::CID_OTHER_LOGIN_VALIDATE_RSP);
    ProxyConn::AddResponsePdu(connUuid, responsePdu);
}

NAMESPACE_END(business)
NAMESPACE_END(z)