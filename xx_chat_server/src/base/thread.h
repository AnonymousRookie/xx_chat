/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: WorkThread
 */

#ifndef BASE_THREAD_H
#define BASE_THREAD_H

#include <list>
#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>
#include "types.h"

class Task;
class WorkThread
{
public:
    WorkThread();
    ~WorkThread();

    void Start();
    void SetStarted(bool started) { started_ = started; }
    void PushTask(const std::shared_ptr<Task>& task);

private:
    void RunInThread();
    void Stop();

private:
    std::thread thread_;
    bool started_{ false };
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::list<std::shared_ptr<Task>> taskList_;
};


#endif  // BASE_THREAD_H