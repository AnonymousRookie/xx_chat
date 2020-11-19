/**
* Copyright 2019-2020, AnonymousRookie. All rights reserved.
* https://github.com/AnonymousRookie/xx_chat
* Author: AnonymousRookie (357688981 at qq dot com)
* Description: 等待提示窗口
*/

#pragma once

#include <memory>
#include <QDialog>
#include <QMovie>
#include <QTimer>
#include "ui_loading_dialog.h"

class LoadingDialog : public QDialog
{
    Q_OBJECT

public:
    LoadingDialog(QWidget *parent = Q_NULLPTR);
    ~LoadingDialog();

    static LoadingDialog* instance();

    void showDlg(int timeout = 1000);
    void hideDlg();

public slots:
    void onLoadMsgTimeout();

private:
    void init();

private:
    Ui::LoadingDialog m_ui;
    std::shared_ptr<QMovie> m_movie = nullptr;
    std::shared_ptr<QTimer> m_spLoadMsgTimer = nullptr;
};
