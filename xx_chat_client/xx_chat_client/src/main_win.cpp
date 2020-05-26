/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "main_win.h"
#include "login\login_module.h"
#include "user_list\user_list_module.h"
#include "protocol\im.login.pb.h"
#include "protocol\im.buddy.pb.h"
#include "protocol\im.base.pb.h"
#include "network\im_pdu_base.h"
#include "util\string_util.h"

MainWin::MainWin(QWidget *parent /*= Q_NULLPTR*/)
    : QWidget(parent)
{
    ui.setupUi(this);
    z::login::GetLoginModule()->AddObserver(this);
    z::user_list::GetUserListModule()->AddObserver(this);
}

MainWin::~MainWin()
{

}

void MainWin::OnNotify(EventId eventId, std::shared_ptr<ImPdu> pdu)
{
    switch (eventId)
    {
    case EventId_LoginDone:
        OnLoginDone(pdu);
        break;
    case EventId_FriendList:
        OnFriendList(pdu);
        break;
    default:
        break;
    }
}

void MainWin::OnLoginDone(std::shared_ptr<ImPdu> pdu)
{
    this->show();
    im::login::LoginRes loginRes;
    bool ret = loginRes.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength());
    Z_CHECK(ret);
    ui.userNameLabel->setText(z::utils::str2qstr(loginRes.user_info().user_name()));
}

void MainWin::OnFriendList(std::shared_ptr<ImPdu> pdu)
{
    im::buddy::AllUserRsp allUserRsp;
    bool ret = allUserRsp.ParseFromArray(pdu->GetBodyData(), pdu->GetBodyLength());
    Z_CHECK(ret);
    allUserRsp.user_info_list();

    uint32_t userCnt = allUserRsp.user_info_list_size();
    for (uint32_t i = 0; i < userCnt; ++i) {
        auto& userInfo = allUserRsp.user_info_list(i);
        userInfo.user_id();
        userInfo.user_name();
    }
}
