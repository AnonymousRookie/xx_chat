/**
 * Copyright 2018-2019, AnonymousRookie. All rights reserved.
 * https://github.com/AnonymousRookie/xx_chat
 * Author: AnonymousRookie (357688981 at qq dot com)
 * Description: 所有连接对象的基类ImConn
 */

#include "im_conn.h"
#include "utils.h"
#include "defines.h"

static ImConnPtr FindImConn(ConnMap* imconn_map, net_handle_t handle)
{
    Z_CHECK_RETURN(imconn_map, nullptr);
    auto iter = imconn_map->find(handle);
    if (iter != imconn_map->end()) {
        return iter->second;
    }
    return nullptr;
}

void ImConnCallback(uint8_t msg, uint32_t handle, void* callbackData)
{
    if (!callbackData) {
        printf("[ImConnCallback] callbackData is null!\n");
        return;
    }

    ConnMap* conn_map = (ConnMap*)callbackData;
    auto pConn = FindImConn(conn_map, handle);
    if (!pConn) {
        printf("[ImConnCallback] cannot find conn, handle=%d\n", handle);
        return;
    }

    switch (msg)
    {
    case NETLIB_MSG_CONFIRM:
        pConn->OnConfirm();
        break;
    case NETLIB_MSG_READ:
        pConn->OnRead();
        break;
    case NETLIB_MSG_WRITE:
        pConn->OnWrite();
        break;
    case NETLIB_MSG_CLOSE:
        pConn->OnClose();
        break;
    default:
        printf("!!!ImConnCallback error msg: %d\n", msg);
        break;
    }
}

ImConn::ImConn() 
    : busy_(false)
    , handle_(NETLIB_INVALID_HANDLE)
    , recvBytes_(0)
{
    lastSendTick_ = z::utils::GetTickCount();
    lastRecvTick_ = z::utils::GetTickCount();
}

ImConn::~ImConn()
{
}

int ImConn::Send(void* data, int len)
{
    lastSendTick_ = z::utils::GetTickCount();

    if (busy_) {
        outBuf_.Write(data, len);
        return len;
    }

    int offset = 0;
    int remain = len;
    while (remain > 0) {
        int send_size = remain;
        if (send_size > NETLIB_MAX_SOCKET_BUF_SIZE) {
            send_size = NETLIB_MAX_SOCKET_BUF_SIZE;
        }
        int ret = z::net::netlib_send(handle_, (char*)data + offset , send_size);
        if (ret <= 0) {
            ret = 0;
            break;
        }
        offset += ret;
        remain -= ret;
    }

    if (remain > 0) {
        outBuf_.Write((char*)data + offset, remain);
        busy_ = true;
        printf("send busy, remain=%d\n", outBuf_.GetWriteOffset());
    } else {
        OnWriteCompelete();
    }

    return len;
}

int ImConn::SendPdu(ImPdu* pdu)
{
    return Send(pdu->GetBuffer(), pdu->GetLength()); 
}

int ImConn::SendPdu(ImPduPtr pdu)
{
    return Send(pdu->GetBuffer(), pdu->GetLength());
}

void ImConn::OnRead()
{
    for (;;)
    {
        uint32_t free_buf_len = inBuf_.GetAllocSize() - inBuf_.GetWriteOffset();
        if (free_buf_len < READ_BUF_SIZE) {
            inBuf_.Extend(READ_BUF_SIZE);
        }

        int ret = z::net::netlib_recv(handle_, inBuf_.GetBuffer() + inBuf_.GetWriteOffset(), READ_BUF_SIZE);
        if (ret <= 0) {
            break;
        }

        recvBytes_ += ret;
        inBuf_.IncWriteOffset(ret);

        lastRecvTick_ = z::utils::GetTickCount();
    }

    ImPdu* pPdu = NULL;
    try {
        while ((pPdu = ImPdu::ReadPdu(inBuf_.GetBuffer(), inBuf_.GetWriteOffset()))) {
            uint32_t pdu_len = pPdu->GetLength();
            HandlePdu(pPdu);
            inBuf_.Read(NULL, pdu_len);
            delete pPdu;
            pPdu = NULL;
        }
    } catch (PduException& ex) {
        printf("!!!catch exception, sid=%u, cid=%u, err_code=%u, err_msg=%s, close the connection ",
                ex.GetServiceId(), ex.GetCommandId(), ex.GetErrorCode(), ex.GetErrorMsg());
        if (pPdu) {
            delete pPdu;
            pPdu = NULL;
        }
        OnClose();
    }
}

void ImConn::OnWrite()
{
    if (!busy_) {
        return;
    }

    while (outBuf_.GetWriteOffset() > 0) {
        int send_size = outBuf_.GetWriteOffset();
        if (send_size > NETLIB_MAX_SOCKET_BUF_SIZE) {
            send_size = NETLIB_MAX_SOCKET_BUF_SIZE;
        }

        int ret = z::net::netlib_send(handle_, outBuf_.GetBuffer(), send_size);
        if (ret <= 0) {
            ret = 0;
            break;
        }

        outBuf_.Read(NULL, ret);
    }

    if (0 == outBuf_.GetWriteOffset()) {
        busy_ = false;
    }

    printf("onWrite, remain=%d\n", outBuf_.GetWriteOffset());
}