/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include "base\src\defines.h"
#include "operation.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

class IHttpOperation : public CallbackOperation
{
public:
    IHttpOperation(OperationDelegate& callback);
    virtual ~IHttpOperation();

    inline void Cancel();
    inline bool IsCanceled();

private:
    bool cancel_ = false;
};

NAMESPACE_END(core)
NAMESPACE_END(z)
