// Copyright 2018, AnonymousRookie.  All rights reserved.
// https://github.com/AnonymousRookie/xx_chat
// Author: AnonymousRookie (357688981 at qq dot com)


#include "system_tray.h"

#include <QApplication>
#include <QMenu>
#include <QIcon>
#include <QAction>

#include "global.h"

SystemTrayIcon* SystemTrayIcon::system_tray_ = NULL;

SystemTrayIcon::SystemTrayIcon()
{
    menu_ = new QMenu();
    this->setIcon(QIcon(Global::resourceDir() + "/webqq.ico"));
    this->setContextMenu(menu_);
}

void SystemTrayIcon::addMenu(QAction* act)
{
    menu_->addAction(act);
}

void SystemTrayIcon::addNotifyAction(QAction* act)
{
    setIcon(act->icon());
    menu_->addAction(act);
    actions_.push_back(act);
}

void SystemTrayIcon::removeAction(QAction* act)
{
    menu_->removeAction(act);
    actions_.removeOne(act);

    if (actions_.isEmpty()) {
        setIcon(QIcon(Global::resourceDir() + "/webqq.ico"));
    } else {
        setIcon(actions_.last()->icon());
    }
}

void SystemTrayIcon::addSeparator()
{
    menu_->addSeparator();
}

bool SystemTrayIcon::hasNotify()
{
    return !actions_.isEmpty();
}


