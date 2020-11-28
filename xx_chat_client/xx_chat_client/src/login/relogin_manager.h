/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ∂œœﬂ÷ÿ¡¨
 */

#pragma once

#include <memory>
#include "defines.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

class TimerEvent;

NAMESPACE_END(core)
NAMESPACE_END(z)

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(login)

class ReLoginManager
{
public:
    ReLoginManager();
    ~ReLoginManager();

    static std::shared_ptr<ReLoginManager> GetInstance();
    void DoReLogin();
    
private:
    void ReLogin();
    void OnOperationCallback(std::shared_ptr<void> param);

private:
    static std::shared_ptr<ReLoginManager> instance_;
    std::shared_ptr<z::core::TimerEvent> timerEvent_ = nullptr;
};

NAMESPACE_END(login)
NAMESPACE_END(z)