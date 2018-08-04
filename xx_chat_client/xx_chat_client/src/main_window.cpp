// Copyright 2018, AnonymousRookie.  All rights reserved.
// https://github.com/AnonymousRookie/xx_chat
// Author: AnonymousRookie (357688981 at qq dot com)


#include "main_window.h"
#include "ui_main_window.h"

#include <QDesktopWidget>
#include <QMessageBox>
#include <QEvent>
#include <QCursor>

#include "utils/menu.h"
#include "global.h"
#include "system_tray.h"

MainWindow::MainWindow(QWidget* parent) : QWidget(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);

    setObjectName("mainWindow");
    setWindowIcon(QIcon(Global::instance()->appIconPath()));

    move((QApplication::desktop()->width() - this->width()) /2, (QApplication::desktop()->height() - this->height()) /2);

    main_menu_ = new Menu(this);



    SystemTrayIcon *tray = SystemTrayIcon::instance();
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onTrayIconClicked(QSystemTrayIcon::ActivationReason)));
}


MainWindow::~MainWindow()
{
    if (ui_) {
        delete ui_;
        ui_ = NULL;
    }
}

void MainWindow::onTrayIconClicked( QSystemTrayIcon::ActivationReason reason )
{
    if (reason != QSystemTrayIcon::Trigger) {
        return;
    }
    if (isMinimized() || !isVisible()) {
        showNormal();
    } else {
        hide();
    }
}


void MainWindow::closeEvent(QCloseEvent*)
{

}

void MainWindow::initialize()
{

}
