/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */


#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include "base\src\defines.h"
#include "module_observer.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

class ModuleObserver;

class ModuleSubject final : public std::enable_shared_from_this<ModuleSubject>
{
public:
    ModuleSubject();
    ~ModuleSubject();

public:
    void AddObserver(ModuleObserver* observer);
    void RemoveObserver(ModuleObserver* observer);
    void AsynNotifyObserver(EventId eventId, std::shared_ptr<ImPdu> pdu);
    void AsynNotifyObserver(EventId eventId, void* data, int len);
    std::vector<ModuleObserver*> GetModuleObservers();

private:
    std::vector<ModuleObserver*> moduleObserverVector_;
    std::mutex mutex_;
};


NAMESPACE_END(core)
NAMESPACE_END(z)
