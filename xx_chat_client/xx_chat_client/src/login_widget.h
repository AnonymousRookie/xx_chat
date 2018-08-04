// Copyright 2018, AnonymousRookie.  All rights reserved.
// https://github.com/AnonymousRookie/xx_chat
// Author: AnonymousRookie (357688981 at qq dot com)


#ifndef XX_CHAT_CLIENT_SRC_LOGIN_WIDGET_H
#define XX_CHAT_CLIENT_SRC_LOGIN_WIDGET_H

#include <QWidget>

namespace Ui {
    class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

signals:
    void loginFinished();

public:
    explicit LoginWidget(QWidget* parent = NULL);
    ~LoginWidget();


private slots:
    void login();

private:
    Ui::LoginWidget* ui_;

};



#endif  // XX_CHAT_CLIENT_SRC_LOGIN_WIDGET_H