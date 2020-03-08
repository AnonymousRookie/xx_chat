/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: WorkThread
 */

#include "thread.h"
#include "task.h"
#include "logger.h"

WorkThread::WorkThread()
{

}

WorkThread::~WorkThread()
{
    Stop();
}

void WorkThread::Start()
{
    if (started_) {
        return;
    }
    started_ = true;
    thread_ = std::thread(&WorkThread::RunInThread, this);
}

void WorkThread::Stop()
{
    started_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

void WorkThread::RunInThread()
{
    while (started_) {
        std::unique_lock<std::mutex> lock(mutex_);
        // always use a while-loop, due to spurious wakeup
        while (taskList_.empty()) {
            notEmpty_.wait(lock);
        }
        auto task = taskList_.front();
        taskList_.pop_front();
        task->Run();
    }
}

void WorkThread::PushTask(const std::shared_ptr<Task>& task)
{
    std::unique_lock<std::mutex> lock(mutex_);
    taskList_.push_back(task);
    notEmpty_.notify_one();
}
