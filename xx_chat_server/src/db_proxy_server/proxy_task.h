/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ProxyTask
 */

#ifndef DB_PROXY_SERVER_PROXY_TASK_H
#define DB_PROXY_SERVER_PROXY_TASK_H

#include "types.h"
#include "task.h"

typedef void (*pdu_handler_t)(ImPdu* pdu, uint32_t connUuid);

class ImPdu;

class ProxyTask : public Task
{
public:
    ProxyTask(uint32_t connUuid, pdu_handler_t pduHandler, ImPdu* pdu);
    virtual ~ProxyTask();

    virtual void Run();

private:
    uint32_t connUuid_;
    pdu_handler_t pduHandler_;
    ImPdu* pdu_;
};


#endif  // DB_PROXY_SERVER_PROXY_TASK_H