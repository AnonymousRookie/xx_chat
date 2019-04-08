/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ThreadPool
 */

#include "stdafx.h"
#include "utils.h"
#include "thread_pool.h"
#include "task.h"
#include "thread.h"


ThreadPool::ThreadPool()
{
    workThreadList_.clear();
}

ThreadPool::~ThreadPool()
{

}

int ThreadPool::Init(uint32_t workThreadSize)
{
    for (uint32_t i = 0; i < workThreadSize; ++i) {
        auto workThread = std::make_shared<WorkThread>();
        workThread->Start();
        workThreadList_.push_back(workThread);
    }
    return 0;
}

void ThreadPool::AddTask(const std::shared_ptr<Task>& task)
{
    uint32_t threadIndex = z::utils::Random(static_cast<uint32_t>(0), static_cast<uint32_t>(workThreadList_.size() - 1));
    workThreadList_.at(threadIndex)->PushTask(task);
}
