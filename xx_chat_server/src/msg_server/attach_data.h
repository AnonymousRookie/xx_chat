/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: AttachData
 */

#ifndef MSG_SERVER_ATTACH_DATA_H
#define MSG_SERVER_ATTACH_DATA_H

#include "util_pdu.h"

enum AttachDataType {
    attachDataType_Handle = 1,
};

class AttachData
{
public:
    // 序列化
    AttachData(uint32_t type, uint32_t handle, uint32_t serviceType = 0);
    // 反序列化
    AttachData(uchar_t* attachData, uint32_t attachLen);

    uchar_t* GetBuffer() { return buf_.GetBuffer(); }
    uint32_t GetLength() { return buf_.GetWriteOffset(); }
    uint32_t GetType() { return type_; }
    uint32_t GetHandle() { return handle_; }
    uint32_t GetServiceType() { return serviceType_; }

private:
    SimpleBuffer buf_;
    uint32_t type_;
    uint32_t handle_;
    uint32_t serviceType_;
};


#endif  // MSG_SERVER_ATTACH_DATA_H