/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "operation_manager.h"
#include "operation.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

OperationManager::~OperationManager()
{
    Shutdown();
}

int OperationManager::Startup()
{
    operationThread_ = std::thread([&] 
    {
        std::unique_lock<std::mutex> lock(cvMutex_);
        std::shared_ptr<Operation> operation = nullptr;
        while (running_)
        {
            while (realtimeOperations_.empty()) {
                if (!running_) {
                    break;
                }
                cv_.wait(lock);
            }
            Z_CHECK_BREAK(running_);
            {
                std::lock_guard<std::mutex> lock(operationMutex_);
                Z_CHECK_CONTINUE(!realtimeOperations_.empty());
                operation = realtimeOperations_.front();
                realtimeOperations_.pop_front();
            }
            Z_CHECK_BREAK(running_);
            if (operation) {
                operation->Process();
            }
        }
    });

    return 0;
}

void OperationManager::Shutdown()
{
    running_ = false;
    cv_.notify_one();
    if (operationThread_.joinable()) {
        operationThread_.join();
    }
    std::lock_guard<std::mutex> lock(operationMutex_);
    realtimeOperations_.clear();
}

int OperationManager::StartOperation(std::shared_ptr<Operation> operation, int32_t delay)
{
    Z_CHECK_RETURN(operation, -1);

    std::lock_guard<std::mutex> lock(operationMutex_);
    realtimeOperations_.push_back(operation);
    cv_.notify_one();

    return 0;
}

int OperationManager::StartOperationWithLambda(std::function<void()> operationRun, int32_t delay)
{
    std::shared_ptr<Operation> operation = std::make_shared<LambdaOperation>(operationRun);
    return StartOperation(operation, delay);
}

OperationManager* GetOperationManager()
{
    static OperationManager manager;
    return &manager;
}

NAMESPACE_END(core)
NAMESPACE_END(z)