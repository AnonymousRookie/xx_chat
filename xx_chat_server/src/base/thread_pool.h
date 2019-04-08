/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ThreadPool
 */

#ifndef BASE_THREAD_POOL_H
#define BASE_THREAD_POOL_H

#include <memory>
#include <vector>
#include "types.h"

class Task;
class WorkThread;
class ThreadPool
{
public:
    ThreadPool();
    virtual ~ThreadPool();

    int Init(uint32_t workThreadSize);
    void AddTask(const std::shared_ptr<Task>& task);

private:
    std::vector<std::shared_ptr<WorkThread>> workThreadList_;
};

#endif  // BASE_THREAD_POOL_H