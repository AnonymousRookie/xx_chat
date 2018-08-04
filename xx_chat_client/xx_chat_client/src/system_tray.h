// Copyright 2018, AnonymousRookie.  All rights reserved.
// https://github.com/AnonymousRookie/xx_chat
// Author: AnonymousRookie (357688981 at qq dot com)


#ifndef XX_CHAT_CLIENT_SRC_SYSTEM_TRAY_H
#define XX_CHAT_CLIENT_SRC_SYSTEM_TRAY_H

#include <QSystemTrayIcon>
#include <QMenu>

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    ~SystemTrayIcon() {}

    static SystemTrayIcon* instance() {
        if (!system_tray_) {
            system_tray_ = new SystemTrayIcon();
        }
        return system_tray_;
    }

    void addMenu(QAction* act);
    void addNotifyAction(QAction* act);
    void removeAction(QAction* act);
    void addSeparator();
    bool hasNotify();

private:
    QMenu* menu_;
    QList<QAction*> actions_;

private:
    SystemTrayIcon();
    SystemTrayIcon(const SystemTrayIcon&);
    SystemTrayIcon &operator=(const SystemTrayIcon&);

    static SystemTrayIcon* system_tray_;
};



#endif  // XX_CHAT_CLIENT_SRC_SYSTEM_TRAY_H