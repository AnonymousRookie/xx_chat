/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: Task抽象类
 */

#ifndef BASE_TASK_H
#define BASE_TASK_H

class Task
{
public:
    Task() {}
    virtual ~Task() {}
    virtual void Run() = 0;
};

#endif  // BASE_TASK_H