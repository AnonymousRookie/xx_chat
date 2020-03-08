/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "http_operation.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

IHttpOperation::IHttpOperation(OperationDelegate& callback)
    : CallbackOperation(callback)
{

}

IHttpOperation:: ~IHttpOperation()
{

}

void IHttpOperation::Cancel()
{ 
    cancel_ = true; 
}

bool IHttpOperation::IsCanceled()
{
    return cancel_; 
}

NAMESPACE_END(core)
NAMESPACE_END(z)
