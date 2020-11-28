/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <QWidget>
#include <QTreeWidgetItem>
#include "ui_main_win.h"
#include "core\module_observer.h"
#include "core\event.h"

class ItemModel;

class MainWin : public QWidget, public z::core::ModuleObserver
{
    Q_OBJECT

public:
    MainWin(QWidget *parent = Q_NULLPTR);
    ~MainWin();

private slots:
    void OnItemDoubleClicked(QTreeWidgetItem* item, int i);

private:
    void OnNotify(EventId eventId, std::shared_ptr<ImPdu> pdu);
    void OnNotify(EventId eventId, void* data, uint32_t len);
    void OnLoginDone(std::shared_ptr<ImPdu> pdu);
    void OnFriendList(std::shared_ptr<ImPdu> pdu);
    void OnTcpClientState(void* data, uint32_t len);

private:
    Ui::MainWin ui;
    std::vector<QTreeWidgetItem*> treeWidgetItems_;
};