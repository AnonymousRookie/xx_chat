// Copyright 2018, AnonymousRookie.  All rights reserved.
// https://github.com/AnonymousRookie/xx_chat
// Author: AnonymousRookie (357688981 at qq dot com)


#ifndef XX_CHAT_CLIENT_H
#define XX_CHAT_CLIENT_H

#include <QObject>
#include "ui_xx_chat_client.h"

class LoginWidget;
class MainWindow;
class QAction;

class XXChatClient : public QObject
{
    Q_OBJECT

public:
    ~XXChatClient();

    static XXChatClient* instance() {
        if (!instance_) {
            instance_ = new XXChatClient();
        }
        return instance_;
    }

    MainWindow* mainWindow() const {
        return main_window_;
    }

    void start();


public slots:
    void aboutXXChartClient();

    void showMainPanel();

    void onMinimize();
    void onRestore();
    void onLogout();
    void onQuit();

private:
    LoginWidget* login_widget_;
    MainWindow* main_window_;

    QAction* minimize_;
    QAction* restore_;
    QAction* quit_;


private:
    XXChatClient();
    XXChatClient(const XXChatClient&);
    XXChatClient& operator=(const XXChatClient&);

private:
    static XXChatClient* instance_;
};

#endif // XX_CHAT_CLIENT_H
