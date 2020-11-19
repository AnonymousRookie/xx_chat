/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "operation.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

Operation::Operation()
{

}

Operation::Operation(const std::string& name)
    : name_(name)
{
}

Operation::~Operation()
{
}

void Operation::Process()
{
    state_ = OPERATION_RUNNING;
    ProcessOperation();
    state_ = OPERATION_FINISHED;
}


CallbackOperation::CallbackOperation(OperationDelegate& callback)
    : callback_(callback)
{

}

CallbackOperation::~CallbackOperation()
{

}

void CallbackOperation::SyncCallback(std::shared_ptr<void> param)
{
    callback_(param);
}


LambdaOperation::LambdaOperation(std::function<void()> operationRun)
    : operationRun_(operationRun)
{

}

LambdaOperation::~LambdaOperation()
{
}

void LambdaOperation::ProcessOperation()
{
    operationRun_();
}


NAMESPACE_END(core)
NAMESPACE_END(z)
