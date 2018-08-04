// Copyright 2018, AnonymousRookie.  All rights reserved.
// https://github.com/AnonymousRookie/xx_chat
// Author: AnonymousRookie (357688981 at qq dot com)


#include "xx_chat_client.h"

#include <QMessageBox>

#include "login_widget.h"
#include "main_window.h"
#include "system_tray.h"
#include "global.h"
#include "skin_engine/skin_engine.h"

XXChatClient* XXChatClient::instance_ = NULL;


XXChatClient::XXChatClient() : login_widget_(NULL),
    main_window_(NULL)
{
    SystemTrayIcon* tray  = SystemTrayIcon::instance();

    minimize_ = new QAction(QIcon(SkinEngine::instance()->skinRes("systray_minimize")), tr("Minimize"), NULL);
    connect(minimize_, SIGNAL(triggered()), this, SLOT(onMinimize()));

    restore_ = new QAction(QIcon(SkinEngine::instance()->skinRes("systray_restore")), tr("Restore"), NULL);
    connect(restore_, SIGNAL(triggered()), this, SLOT(onRestore()));


    quit_ = new QAction(QIcon(SkinEngine::instance()->skinRes("systray_quit")), tr("Quit"), NULL);
    connect(quit_, SIGNAL(triggered()), this, SLOT(onQuit()));

    tray->addMenu(minimize_);
    tray->addMenu(restore_);
    tray->addMenu(quit_);
    tray->addSeparator();
}


XXChatClient::~XXChatClient()
{
    if (login_widget_) {
        login_widget_->close();
        login_widget_->deleteLater();
    }
    if (main_window_) {
        main_window_->close();
        main_window_->deleteLater();
    }
}

void XXChatClient::start()
{
    login_widget_ = new LoginWidget();
    connect(login_widget_, SIGNAL(loginFinished()), this, SLOT(showMainPanel()));
}

void XXChatClient::showMainPanel()
{
    main_window_ = new MainWindow();
    main_window_->initialize();
    main_window_->show();

    login_widget_->deleteLater();
    login_widget_ = NULL;

    SystemTrayIcon::instance()->show();
}

void XXChatClient::onMinimize()
{
    main_window_->hide();
}

void XXChatClient::onRestore()
{
    main_window_->showNormal();
}

void XXChatClient::onLogout()
{
    delete main_window_;
    
    SystemTrayIcon *trayicon = SystemTrayIcon::instance();
    trayicon->hide();
    
    start();
}

void XXChatClient::onQuit()
{
    if (main_window_) {
        delete main_window_;
        main_window_ = NULL;
    }

    delete minimize_;
    minimize_ = NULL;

    delete restore_;
    restore_ = NULL;

    delete quit_;
    quit_ = NULL;

    qApp->quit();
}


void XXChatClient::aboutXXChartClient()
{
    QMessageBox::information(NULL, tr("About XXChartClient"), 
        tr("Version:%1\n").arg(Global::version()));
}