/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "base\src\defines.h"
#include "base\src\types.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

class Operation;
class OperationManager
{
public:
    OperationManager() = default;
    ~OperationManager();

    OperationManager(OperationManager&) = delete;
    OperationManager& operator=(OperationManager&) = delete;

public:
    int Startup();
    void Shutdown();
    int StartOperation(std::shared_ptr<Operation> operation, int32_t delay = 0);
    int StartOperationWithLambda(std::function<void()> operationRun, int32_t delay = 0);

private:
    std::list<std::shared_ptr<Operation>> delayOperations_;     // 延时处理的操作
    std::list<std::shared_ptr<Operation>> realtimeOperations_;  // 实时处理的操作

    std::mutex cvMutex_;
    std::condition_variable cv_;

    std::mutex operationMutex_;

    bool running_ = true;
    std::thread operationThread_;
};

OperationManager* GetOperationManager();

NAMESPACE_END(core)
NAMESPACE_END(z)
