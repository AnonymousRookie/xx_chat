// Copyright 2018, AnonymousRookie.  All rights reserved.
// https://github.com/AnonymousRookie/xx_chat
// Author: AnonymousRookie (357688981 at qq dot com)


#include "login_widget.h"
#include "ui_login_widget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>

#include "global.h"

LoginWidget::LoginWidget(QWidget* parent) :
    QWidget(parent),
    ui_(new Ui::LoginWidget())
{
    ui_->setupUi(this);
    
    setObjectName("loginWidget");

    setWindowIcon(QIcon(Global::instance()->appIconPath()));

    connect(ui_->pb_login, SIGNAL(clicked()), this, SLOT(login()));
     
    move((QApplication::desktop()->width() - this->width()) /2, (QApplication::desktop()->height() - this->height()) /2);

    this->show();
}

LoginWidget::~LoginWidget()
{
    if (ui_) {
        delete ui_;
        ui_ = NULL;
    }
}

void LoginWidget::login()
{
    const QString& username = ui_->comb_username_->currentText();
    const QString& password = ui_->le_password_->text();
    bool empty_username = username.isEmpty();
    bool empty_password = password.isEmpty();
    if (empty_username || empty_password) {
        QMessageBox warning_box;
        warning_box.setIcon(QMessageBox::Warning);
        warning_box.setInformativeText(tr("username or password is empty! please input them!"));
        warning_box.exec();
        return;
    }

    ui_->pb_login->setEnabled(false);

    emit loginFinished();

    ui_->pb_login->setEnabled(true);
}

