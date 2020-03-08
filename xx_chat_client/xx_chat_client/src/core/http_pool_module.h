/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>
#include "base\src\defines.h"
#include "base\src\types.h"
#include "http_operation.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

class HttpThread;

class IHttpPoolModule
{
public:
    // 将http操作添加到线程池
    virtual void PushHttpOperation(std::shared_ptr<IHttpOperation> operation, bool highPriority = false) = 0;
    virtual void Shutdown() = 0;
};

class HttpPoolModule final : public IHttpPoolModule
{
public:
    HttpPoolModule();
    virtual ~HttpPoolModule();
    virtual void PushHttpOperation(std::shared_ptr<IHttpOperation> operation, bool highPriority = false);
    virtual void Shutdown();

private:
    virtual void cancelAllOperations();

private:
    void Init();

private:
    std::vector<std::shared_ptr<HttpThread>> httpThreads_;
};

IHttpPoolModule* getHttpPoolModule();


class HttpThread
{
public:
    HttpThread();
    ~HttpThread();

    void Start();
    void SetStarted(bool started) { started_ = started; }
    void PushHttpOperation(std::shared_ptr<IHttpOperation> operation, bool highPriority = false);

private:
    void RunInThread();
    void Stop();

private:
    bool started_ = false;
    std::thread thread_;
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::list<std::shared_ptr<IHttpOperation>> lstHttpOperations_;
};


NAMESPACE_END(core)
NAMESPACE_END(z)
