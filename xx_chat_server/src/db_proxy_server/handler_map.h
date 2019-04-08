/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: HandlerMapManager
 */

#ifndef DB_PROXY_SERVER_HANDLER_MAP_H
#define DB_PROXY_SERVER_HANDLER_MAP_H

#include <map>
#include "utils.h"
#include "proxy_task.h"

// commandId和对应的处理函数
typedef std::map<uint32_t, pdu_handler_t> HandlerMap;

class HandlerMapManager
{
public:
    virtual ~HandlerMapManager();
    static HandlerMapManager* GetInstance();

    void Init();
    pdu_handler_t GetHandler(uint32_t pduType);

private:
    HandlerMapManager();

private:
    static HandlerMapManager* handlerMapManagerInstance_;
    HandlerMap handlerMap_;
};


#endif  // DB_PROXY_SERVER_HANDLER_MAP_H