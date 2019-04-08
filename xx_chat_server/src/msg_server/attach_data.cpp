/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: AttachData
 */

#include "stdafx.h"
#include "attach_data.h"

AttachData::AttachData(uint32_t type, uint32_t handle, uint32_t serviceType)
{
    ByteStream os(&buf_, 0);
    os << type;
    os << handle;
    os << serviceType;
}

AttachData::AttachData(uchar_t * attachData, uint32_t attachLen)
{
    ByteStream is(attachData, attachLen);
    is >> type_;
    is >> handle_;
    is >> serviceType_;
}
