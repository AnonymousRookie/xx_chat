/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: ProxyTask
 */

#include "stdafx.h"
#include "im_pdu_base.h"
#include "proxy_task.h"
#include "proxy_conn.h"

ProxyTask::ProxyTask(uint32_t connUuid, pdu_handler_t pduHandler, std::shared_ptr<ImPdu> pdu)
{
    connUuid_ = connUuid;
    pduHandler_ = pduHandler;
    pdu_ = pdu;
}

ProxyTask::~ProxyTask()
{

}

void ProxyTask::Run()
{
    if (!pdu_) {
        // tell ProxyConn to close connection with connUuid_
        ProxyConn::AddResponsePdu(connUuid_, NULL);
    } else {
        if (pduHandler_) {
            pduHandler_(pdu_, connUuid_);
        }
    }
}
