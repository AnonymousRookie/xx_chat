/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "main_win.h"
#include "login\login_module.h"
#include "login\relogin_manager.h"
#include "user_list\user_list_module.h"
#include "protocol\im.login.pb.h"
#include "protocol\im.buddy.pb.h"
#include "protocol\im.base.pb.h"
#include "network\im_pdu_base.h"
#include "util\string_util.h"
#include "chat\chat_dlg_manager.h"
#include "data_manager.h"
#include "core\tcp_client_module.h"

MainWin::MainWin(QWidget *parent /*= Q_NULLPTR*/)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.treeWidget_contacts->setHeaderHidden(true);
    ui.treeWidget_contacts->clear();

    connect(ui.treeWidget_contacts, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this,
        SLOT(OnItemDoubleClicked(QTreeWidgetItem*, int)));

    z::login::GetLoginModule()->AddObserver(this);
    z::user_list::GetUserListModule()->AddObserver(this);
    z::core::GetTcpClientModule()->AddObserver(this);
}

MainWin::~MainWin()
{
    for (auto item : treeWidgetItems_)
    {
        if (item) {
            delete item;
            item = NULL;
        }
    }
}

void MainWin::OnItemDoubleClicked(QTreeWidgetItem* item, int i)
{
    Z_CHECK(item);
    auto& name = item->text(i);

    ChatDlgManager::Instance()->OpenFriendChatDlg(name);
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

void MainWin::OnNotify(EventId eventId, void* data, uint32_t len)
{
    switch (eventId)
    {
    case EventId_TcpClientState:
        OnTcpClientState(data, len);
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
    uint32_t userId = loginRes.user_info().user_id();
    DataManager::GetInstance()->SetCurLoginUserId(userId);
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

        DataManager::GetInstance()->AddUserInfo(UserInfo(userInfo.user_id(), userInfo.user_name()));

        QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(ui.treeWidget_contacts);
        treeWidgetItems_.push_back(treeWidgetItem);
        treeWidgetItem->setText(0, z::utils::str2qstr(userInfo.user_name()));
    }
}

void MainWin::OnTcpClientState(void* data, uint32_t len)
{
    uint8_t state = z::core::TcpClientState_Invalid;
    if (len >= sizeof(state))
    {
        state = *(uint8_t*)data;
    }
    if (z::core::TcpClientState_DisConnected == state)
    {
        // 发起重连
        z::login::ReLoginManager::GetInstance()->DoReLogin();
    }
}
