/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: µÇÂ¼½çÃæ
 */

#pragma once

#include <string>
#include <memory>
#include <QWidget>
#include "defines.h"
#include "ui_login_win.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(login)

class LoginWin : public QWidget
{
    Q_OBJECT

public:
    LoginWin(QWidget *parent = Q_NULLPTR);
    ~LoginWin();

public:
    void OnHttpLoginCallback(std::shared_ptr<void> param);
    void OnOperationCallback(std::shared_ptr<void> param);

private slots:
    void DoLogin();

private:
    std::string username_;
    std::string password_;

private:
    Ui::LoginWin ui;
};

NAMESPACE_END(login)
NAMESPACE_END(z)