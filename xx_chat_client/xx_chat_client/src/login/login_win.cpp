/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 登录界面
 */

#include <string>
#include <memory>
#include <QMessageBox>
#include "util\logger.h"
#include "network\net_lib.h"
#include "util\string_util.h"
#include "..\core\http_pool_module.h"
#include "..\core\operation_manager.h"
#include "login_http_server_operation.h"
#include "login_operation.h"
#include "login_win.h"
#include "login_module.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(login)

LoginWin::LoginWin(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.pushButton_login, SIGNAL(clicked()), this, SLOT(DoLogin()));
    this->show();
    z::login::GetLoginModule()->AddObserver(this);
}

LoginWin::~LoginWin()
{
    z::net::netlib_stop_event();
}

void LoginWin::DoLogin()
{
    if (ui.comboBox_username->currentText().isEmpty() || ui.lineEdit_password->text().isEmpty()) {
        QMessageBox box(this);
        box.setIcon(QMessageBox::Critical);
        box.setInformativeText("username or password is empty!");
        box.exec();
        return;
    }

    username_ = z::utils::qStr2Str(ui.comboBox_username->currentText());
    password_ = z::utils::qStr2Str(ui.lineEdit_password->text());

    // 连接login_server
    DoLoginServerParam param;
    auto operation = std::make_shared<LoginHttpServerOperation>(
        std::bind(&LoginWin::OnHttpLoginCallback, this, std::placeholders::_1), param);
    z::core::getHttpPoolModule()->PushHttpOperation(operation);
}

void LoginWin::OnHttpLoginCallback(std::shared_ptr<void> param)
{
    DoLoginServerParam* pParam = (DoLoginServerParam*)param.get();

    // 连接login_server成功
    if (LOGIN_LOGIN_SERVER_SUCCESS == pParam->result)
    {
        LOG_INFO("%s连接login_server成功!", username_.c_str());

        // 开始连接msg_server
        LoginMsgServerParam loginParam;
        loginParam.username = username_;
        loginParam.password = password_;
        loginParam.msgServerIp = pParam->msgServerIp;
        loginParam.msgServerPort = pParam->msgServerPort;

        auto pOperation = std::make_shared<LoginOperation>(
            std::bind(&LoginWin::OnOperationCallback, this, std::placeholders::_1), loginParam);
        z::core::GetOperationManager()->StartOperation(pOperation, 0);
    }
    else
    {
        // 连接login_server失败
        LOG_WARN("%s连接login_server失败!", username_.c_str());
    }
}

void LoginWin::OnOperationCallback(std::shared_ptr<void> param)
{

}

void LoginWin::OnNotify(EventId eventId, std::shared_ptr<ImPdu> pdu)
{
    // 登录成功, 隐藏登录窗口
    if (EventId_LoginDone == eventId)
    {
        hide();
    }
}

NAMESPACE_END(login)
NAMESPACE_END(z)
