/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <memory>
#include "base\src\defines.h"
#include "module_observer.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

class ModuleSubject;

class ModuleBase
{
public:
    ModuleBase();
    ~ModuleBase();

public:
    void AddObserver(ModuleObserver* observer);
    void RemoveObserver(ModuleObserver* observer);
    void AsynNotifyObserver(EventId keyId, std::shared_ptr<ImPdu> pdu);

private:
    std::shared_ptr<ModuleSubject> moduleSubjectSptr_;
};

NAMESPACE_END(core)
NAMESPACE_END(z)
