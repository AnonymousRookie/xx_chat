// Copyright 2018, AnonymousRookie.  All rights reserved.
// https://github.com/AnonymousRookie/xx_chat
// Author: AnonymousRookie (357688981 at qq dot com)


#ifndef XX_CHAT_CLIENT_SRC_MAIN_WINDOW_H
#define XX_CHAT_CLIENT_SRC_MAIN_WINDOW_H

#include <QWidget>
#include <QCloseEvent>
#include <QSystemTrayIcon>

namespace Ui
{
    class MainWindow;
}

class Menu;


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = NULL);
    ~MainWindow();

public:
    void initialize();


    // 更新托盘显示的用户信息
    void updateLoginUser() const;
    void clean();

    Menu* mainMenu() const {
        return main_menu_;
    }

protected:
    virtual void closeEvent(QCloseEvent*);


private slots:
    void onTrayIconClicked(QSystemTrayIcon::ActivationReason reason);


private:
    Ui::MainWindow* ui_;

    Menu* main_menu_;

};




#endif  // XX_CHAT_CLIENT_SRC_MAIN_WINDOW_H