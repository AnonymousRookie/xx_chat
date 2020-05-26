/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ��¼����
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

    // ����login_server
    DoLoginServerParam param;
    auto operation = std::make_shared<LoginHttpServerOperation>(
        std::bind(&LoginWin::OnHttpLoginCallback, this, std::placeholders::_1), param);
    z::core::getHttpPoolModule()->PushHttpOperation(operation);
}

void LoginWin::OnHttpLoginCallback(std::shared_ptr<void> param)
{
    DoLoginServerParam* pParam = (DoLoginServerParam*)param.get();

    // ����login_server�ɹ�
    if (LOGIN_LOGIN_SERVER_SUCCESS == pParam->result)
    {
        LOG_INFO("%s����login_server�ɹ�!", username_.c_str());

        // ��ʼ����msg_server
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
        // ����login_serverʧ��
        LOG_WARN("%s����login_serverʧ��!", username_.c_str());
    }
}

void LoginWin::OnOperationCallback(std::shared_ptr<void> param)
{

}

NAMESPACE_END(login)
NAMESPACE_END(z)