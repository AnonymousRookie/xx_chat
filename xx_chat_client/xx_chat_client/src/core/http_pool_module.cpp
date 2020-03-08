/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#include "base\src\util\utils.h"
#include "http_pool_module.h"

NAMESPACE_BEGIN(z)
NAMESPACE_BEGIN(core)

IHttpPoolModule* getHttpPoolModule()
{
    static HttpPoolModule module;
    return &module;
}


HttpPoolModule::HttpPoolModule()
{
    Init();
}

HttpPoolModule::~HttpPoolModule()
{
    Shutdown();
}

void HttpPoolModule::PushHttpOperation(std::shared_ptr<IHttpOperation> operation, bool highPriority)
{
    Z_CHECK(operation);
    uint32_t threadIndex = z::utils::Random(static_cast<uint32_t>(0), static_cast<uint32_t>(httpThreads_.size() - 1));
    httpThreads_.at(threadIndex)->PushHttpOperation(operation, highPriority);
}

void HttpPoolModule::Shutdown()
{
    httpThreads_.clear();
}

void HttpPoolModule::cancelAllOperations()
{
}

void HttpPoolModule::Init()
{
    enum { MAX_HTTP_THREAD_NUM = 4 };
    for (int i = 0; i < MAX_HTTP_THREAD_NUM; ++i) {
        auto httpThread = std::make_shared<HttpThread>();
        httpThread->Start();
        httpThreads_.push_back(httpThread);
    }
}


HttpThread::HttpThread()
{
}

HttpThread::~HttpThread()
{
    Stop();
}

void HttpThread::Start()
{
    if (started_) {
        return;
    }
    started_ = true;
    thread_ = std::thread(&HttpThread::RunInThread, this);
}

void HttpThread::PushHttpOperation(std::shared_ptr<IHttpOperation> operation, bool highPriority)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (highPriority) {
        lstHttpOperations_.push_front(operation);
    }
    else {
        lstHttpOperations_.push_back(operation);
    }
    notEmpty_.notify_one();
}

void HttpThread::RunInThread()
{
    while (started_) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (lstHttpOperations_.empty()) {
            if (!started_) {
                return;
            }
            notEmpty_.wait(lock);
        }
        auto operation = lstHttpOperations_.front();
        lstHttpOperations_.pop_front();
        operation->Process();
    }
}

void HttpThread::Stop()
{
    started_ = false;
    notEmpty_.notify_one();
    if (thread_.joinable()) {
        thread_.join();
    }
    std::unique_lock<std::mutex> lock(mutex_);
    lstHttpOperations_.clear();
}


NAMESPACE_END(core)
NAMESPACE_END(z)

