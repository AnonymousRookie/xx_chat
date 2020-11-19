/**
 * Copyright 2019-2020, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: NONE
 */

#pragma once

#include <string>
#include <cstdint>
#include "defines.h"

enum MSG_TYPE
{
    MSG_TYPE_TEXT_P2P = 0x01,    // 端对端消息
    MSG_TYPE_TEXT_GROUP = 0x11,  // 群组消息
};

class MessageEntity
{
public:
    MessageEntity();
    ~MessageEntity();

public:
    uint32_t msgId_;
    uint8_t msgType_;         // 消息类型
    uint32_t msgTime_;        // 消息收发时间
    std::string msgContent_;  // 消息内容
    uint32_t from_;
    uint32_t to_;      
};



