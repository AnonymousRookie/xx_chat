/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "../stdafx.h"
#include "im.buddy.pb.h"
#include "../proxy_conn.h"
#include "user_model.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(business)

void GetAllUser(std::shared_ptr<ImPdu> pdu, uint32_t connUuid)
{
    ImPduPtr responsePdu = std::make_shared<ImPdu>();

    im::buddy::AllUserReq reqMsg;
    im::buddy::AllUserRsp rspMsg;

    if (reqMsg.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength())) {
        uint32_t reqUserId = reqMsg.user_id();

        std::list<im::base::UserInfo> userInfoList;
        UserModel::GetInstance()->GetUserInfos(userInfoList);

        rspMsg.set_user_id(reqUserId);
        auto iter = userInfoList.begin();
        for (; iter != userInfoList.end(); ++iter) {
            im::base::UserInfo* userInfo = rspMsg.add_user_info_list();
            userInfo->set_user_id(iter->user_id());
            userInfo->set_user_name(iter->user_name());
        }
        rspMsg.set_attach_data(reqMsg.attach_data());
        responsePdu->SetPBMsg(&rspMsg);
        responsePdu->SetSeqNum(pdu->GetSeqNum());
        responsePdu->SetServiceId(im::base::SID_BUDDY_LIST);
        responsePdu->SetCommandId(im::base::CID_BUDDY_LIST_ALL_USER_RSP);
        ProxyConn::AddResponsePdu(connUuid, responsePdu);
    }
}

NAMESPACE_END(business)
NAMESPACE_END(z)