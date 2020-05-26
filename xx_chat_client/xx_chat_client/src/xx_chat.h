/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <memory>
#include <QWidget>
#include "login\login_win.h"
#include "main_win.h"

class MainWin;

class XXChat : public QWidget
{
    Q_OBJECT

public:
    ~XXChat();

public:
    static XXChat* GetInstance()
    {
        if (!instance_) {
            instance_ = new XXChat();
        }
        return instance_;
    }

    void Start();

private:
    XXChat();
    XXChat(const XXChat&) = delete;
    XXChat& operator=(const XXChat&) = delete;

private:
    static XXChat* instance_;
    MainWin* mainWin_;
};