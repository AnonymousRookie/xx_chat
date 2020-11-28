/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "util\logger.h"
#include "..\core\operation_manager.h"
#include "..\core\tcp_client_module.h"
#include "..\protocol\im.buddy.pb.h"
#include "..\protocol\im.login.pb.h"
#include "login_module.h"
#include "login_win.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(login)

ILoginModule* GetLoginModule()
{
    static LoginModule module;
    return &module;
}

LoginModule::LoginModule()
{
}

LoginModule::~LoginModule()
{
}

void LoginModule::OnPacket(std::shared_ptr<ImPdu> pdu)
{
    im::login::LoginRes loginRes;
    uint32_t seqNo = pdu->GetSeqNum();

    bool ret = loginRes.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength());
    Z_CHECK(ret);

    uint32_t resultCode = loginRes.result_code();
    std::string resultMsg = loginRes.result_string();

    if (resultCode != 0) {
        return;
    }

    NotifyLoginDone(pdu);
}

bool LoginModule::ShowLoginDialog()
{
    loginDlg_ = std::make_shared<LoginWin>();
    if (loginDlg_) {
        loginDlg_->show();
    }
    return true;
}

void LoginModule::NotifyLoginDone(std::shared_ptr<ImPdu> pdu)
{
    im::login::LoginRes loginRes;
    uint32_t seqNo = pdu->GetSeqNum();

    bool ret = loginRes.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength());
    Z_CHECK(ret);

    // 通知服务器客户端初始化完毕
    z::core::GetOperationManager()->StartOperationWithLambda(
        [=] ()
    {
        // 获取好友信息
        im::buddy::AllUserReq allUserReq;
        allUserReq.set_user_id(loginRes.user_info().user_id());
        z::core::GetTcpClientModule()->SendPacket(im::base::ServiceID::SID_BUDDY_LIST,
            im::base::BuddyListCmdID::CID_BUDDY_LIST_ALL_USER_REQ, &allUserReq);
    });

    z::login::GetLoginModule()->AsynNotifyObserver(EventId::EventId_LoginDone, pdu);
}

NAMESPACE_END(login)
NAMESPACE_END(z)