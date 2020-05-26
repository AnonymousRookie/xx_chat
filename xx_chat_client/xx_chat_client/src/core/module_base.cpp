/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "module_base.h"
#include "module_subject.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

ModuleBase::ModuleBase()
{
    moduleSubjectSptr_ = std::make_shared<ModuleSubject>();
}

ModuleBase::~ModuleBase()
{

}

void ModuleBase::AddObserver(ModuleObserver* observer)
{
    moduleSubjectSptr_->AddObserver(observer);
}

void ModuleBase::RemoveObserver(ModuleObserver* observer)
{
    moduleSubjectSptr_->RemoveObserver(observer);
}

void ModuleBase::AsynNotifyObserver(EventId keyId, std::shared_ptr<ImPdu> pdu)
{
    moduleSubjectSptr_->AsynNotifyObserver(keyId, pdu);
}


NAMESPACE_END(core)
NAMESPACE_END(z)